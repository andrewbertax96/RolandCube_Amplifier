#add_subdirectory(headless)

target_sources(RolandCube PRIVATE
	myLookAndFeel.cpp
	myLookAndFeel.h
	PluginEditor.cpp
	PluginEditor.h
	PluginProcessor.cpp
	PluginProcessor.h
	RTNeuralLSTM.cpp
	RTNeuralLSTM.h
        CabSim.h
        Eq4Band.cpp
        Eq4Band.h
)

#target_precompile_headers(Proteus PRIVATE pch.h)
