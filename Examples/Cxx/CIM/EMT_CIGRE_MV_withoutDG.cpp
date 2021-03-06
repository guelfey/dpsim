#include "cps/CIM/Reader.h"
#include <DPsim.h>
#include <cps/CSVReader.h>

using namespace std;
using namespace DPsim;
using namespace CPS;
using namespace CPS::CIM;

int main(int argc, char** argv){

	// Simulation parameters
	String simName = "EMT_CIGRE_MV_withoutDG";
	Real systemFrequency = 50;
	std::list<fs::path> filenames;
	Real timeStep;
	Real finalTime;
		
	// Set remaining simulation parameters using default values or command line infos
	std::cout<<std::experimental::filesystem::current_path()<<std::endl;
	CommandLineArgs args(argc, argv);
	if (argc <= 1) {
		filenames = DPsim::Utils::findFiles({
			"Rootnet_FULL_NE_28J17h_DI.xml",
			"Rootnet_FULL_NE_28J17h_EQ.xml",
			"Rootnet_FULL_NE_28J17h_SV.xml",
			"Rootnet_FULL_NE_28J17h_TP.xml"
		}, "dpsim/Examples/CIM/grid-data/CIGRE_MV/NEPLAN/CIGRE_MV_no_tapchanger_noLoad1_LeftFeeder_With_LoadFlow_Results", "CIMPATH");
		timeStep = 1e-3;
		finalTime = 0.1;
	}
	else {
		filenames = args.positionalPaths();
		timeStep = args.timeStep;
		finalTime = args.duration;
	}

	// ----- POWERFLOW FOR INITIALIZATION -----
	String simNamePF = simName + "_Powerflow";
	Logger::setLogDir("logs/" + simNamePF);
    CIM::Reader reader(simNamePF, Logger::Level::debug, Logger::Level::debug);
    SystemTopology systemPF = reader.loadCIM(systemFrequency, filenames, CPS::Domain::SP);

    auto loggerPF = DPsim::DataLogger::make(simNamePF);
    for (auto node : systemPF.mNodes)
    {
        loggerPF->addAttribute(node->name() + ".V", node->attribute("v"));
    }
    Simulation simPF(simNamePF, systemPF, 1, 2, Domain::SP, Solver::Type::NRP, Logger::Level::debug, true);
    simPF.addLogger(loggerPF);
    simPF.run();

	// ----- DYNAMIC SIMULATION -----
	Logger::setLogDir("logs/" + simName);
	CIM::Reader reader2(simName, Logger::Level::debug, Logger::Level::debug);
    SystemTopology systemEMT = reader2.loadCIM(systemFrequency, filenames, CPS::Domain::EMT, PhaseType::ABC);
	reader2.initDynamicSystemTopologyWithPowerflow(systemPF, systemEMT);

	auto logger = DPsim::DataLogger::make(simName);
	
	// log node voltages
	for (auto node : systemEMT.mNodes)
		logger->addAttribute(node->name() + ".V", node->attribute("v"));

	// log line currents
	for (auto comp : systemEMT.mComponents) {
		if (dynamic_pointer_cast<CPS::EMT::Ph3::PiLine>(comp))
			logger->addAttribute(comp->name() + ".I", comp->attribute("i_intf"));
	}
	
	// log load currents
	for (auto comp : systemEMT.mComponents) {
		if (dynamic_pointer_cast<CPS::EMT::Ph3::RXLoad>(comp))
			logger->addAttribute(comp->name() + ".I", comp->attribute("i_intf"));
	}

	Simulation sim(simName, systemEMT, timeStep, finalTime, Domain::EMT, Solver::Type::MNA, Logger::Level::debug, true);

	sim.doSteadyStateInit(false);
	sim.addLogger(logger);
	sim.run();

}