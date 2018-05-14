import os
import dpsim as dps
import dpsim.components.dp as dp
import dataprocessing.readtools as rt
import dataprocessing.timeseries as ts

PATH = os.path.dirname(__file__)

def test_IdealVS_RXLine_1_RL():
    # Nodes
    gnd = dps.Node.GND()
    n1 = dps.Node("n1")
    n2 = dps.Node("n2")
    n3 = dps.Node("n3")

    # Components
    v1 = dp.VoltageSource("v_1", [gnd, n1], 10)
    lL = dp.Inductor("l_L", [n2, n3], 0.001)
    rL = dp.Resistor("r_L", [n1, n2], 0.1)
    r1 = dp.Resistor("r_1", [n3, gnd], 20)

    system = dps.SystemTopology(50, [gnd, n1, n2, n3], [v1, lL, rL, r1])

    sim = dps.Simulation('IdealVS_RXLine_1_RL', system, duration=0.2, timestep=0.00005)
    sim.run()

    #results = rt.read_timeseries_dpsim_cmpl('Logs/' + sim.name + '_LeftVector.csv')
    #expected = rt.read_timeseries_dpsim_real('Examples/Results/Simulink/Circuits/SL_' + sim.name() + '.csv')

    err = 0
    #err += ts.TimeSeries.rmse(expected[0], results[0].dynphasor_shift_to_emt('n1_emt', 50))
    #err += ts.TimeSeries.rmse(expected[1], results[1].dynphasor_shift_to_emt('n2_emt', 50))

    print("Total RMSE: %g" % (err))

    assert err < 1e-4

if __name__ == "__main__":
    test_IdealVS_RXLine_1_RL()