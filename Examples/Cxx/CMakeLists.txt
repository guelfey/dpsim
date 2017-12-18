include_directories(
	${EIGEN3_INCLUDE_DIR}
	"${CMAKE_CURRENT_BINARY_DIR}/../../Source"
	"${CMAKE_CURRENT_SOURCE_DIR}/../../Source"
)

if (NOT WIN32)
	# needed so CIMParser and arabica (which are first compiled into static libs)
	# can be included in the .so
	set(CMAKE_CXX_FLAGS "-fPIC")
	# we use PyArg_ParseTupleAndKeywords several times, which erroneously accepts
	# a char* array instead of a const char* array
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-write-strings")
endif()

# enable C++11 support
set(CMAKE_CXX_STANDARD 11)

# targets
set(REFERENCE_SOURCES
	ReferenceCircuits/DpEmtVarFreqStudy.cpp
	ReferenceCircuits/DpEmtVarFreqStudy_NZ_Paper.cpp
	ReferenceCircuits/Example_1.cpp
	ReferenceCircuits/Example_1L2.cpp
	ReferenceCircuits/Example_2.cpp
	ReferenceCircuits/Example_3.cpp
	ReferenceCircuits/Example_IdealVS1.cpp
	ReferenceCircuits/Example_IdealVS2.cpp
	ReferenceCircuits/Example_IdealVS3.cpp
	ReferenceCircuits/Example_IdealVS_EMT.cpp
	ReferenceCircuits/Example_PiLine1.cpp
	ReferenceCircuits/Example_PiLine2.cpp
	ReferenceCircuits/Example_RxLine1.cpp
	ReferenceCircuits/Example_RxLine2.cpp
	ReferenceCircuits/Example_RxLine3.cpp
	ReferenceCircuits/Example_RxLineResLoad.cpp
	ReferenceCircuits/Example_RxLineResLoad_EMT.cpp
)

set(SYNCGEN_SOURCES
	SynchronGenerators/SimpSynGenThreePhaseFault.cpp
	SynchronGenerators/SynGenDPBalancedResLoad.cpp
	SynchronGenerators/SynGenDPThreePhaseFault.cpp
	SynchronGenerators/SynGenBalancedResLoad.cpp
	SynchronGenerators/SynGenPhaseToPhaseFault.cpp
	SynchronGenerators/SynGenThreePhaseFault.cpp
	SynchronGenerators/SynGenVBR.cpp
	SynchronGenerators/SynGenVBRDP.cpp
)

set(RT_SOURCES
	ReferenceCircuits/Example_RT.cpp
)

set(SHMEM_SOURCES
	Shmem/ShmemDistributed.cpp
	Shmem/ShmemDistributedDirect.cpp
	Shmem/ShmemDistributedRef.cpp
	Shmem/ShmemExample.cpp
	Shmem/ShmemMain.cpp
	Shmem/ShmemRTExample.cpp
)

foreach(SOURCE ${REFERENCE_SOURCES} ${SYNCGEN_SOURCES})
	get_filename_component(TARGET ${SOURCE} NAME_WE)

	add_executable(${TARGET} ${SOURCE})

	target_link_libraries(${TARGET} dpsim)
endforeach()

# CIMpp
find_package(CIMpp)
if(CIMpp_FOUND)
	add_executable(CIM CIM.cpp)

	target_include_directories(CIM PUBLIC ${CIMPP_INCLUDE_DIRS})
	target_link_libraries(CIM ${CIMPP_LIBRARIES})
	target_link_libraries(CIM dpsim)
endif()

# Linux for shared memory and real-time
if(LINUX)
	foreach(SOURCE ${SHMEM_SOURCES} ${RT_SOURCES})
		get_filename_component(TARGET ${SOURCE} NAME_WE)

		add_executable(${TARGET} ${SOURCE})

		target_link_libraries(${TARGET} SHARED dpsim)
	endforeach()
endif()