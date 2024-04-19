cmake_minimum_required(VERSION 3.15)
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.12" CACHE STRING "Minimum OS X deployment target")  

project(RolandCube VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 17)  #aggiunto
add_subdirectory(modules/JUCE ./JUCE)
add_subdirectory(modules/chowdsp_utils ./chowdsp_utils)
add_subdirectory(modules/json ./json)

set(RTNEURAL_STL ON CACHE BOOL "Use RTNeural with XSMID backend" FORCE)
add_subdirectory(modules/RTNeural ./RTNeural)
include_directories(modules/RTNeural ./RTNeural)
                   
juce_add_plugin(RolandCube
    COMPANY_NAME Roland                          
    COPY_PLUGIN_AFTER_BUILD TRUE       
    PLUGIN_MANUFACTURER_CODE Brtx               
    PLUGIN_CODE RoCu                            
                                                
    FORMATS AU VST3 Standalone                  
    PRODUCT_NAME "RolandCube")        

juce_generate_juce_header(RolandCube)

target_sources(RolandCube
    PRIVATE
	src/myLookAndFeel.cpp
	src/myLookAndFeel.h
	src/PluginEditor.cpp
	src/PluginEditor.h
	src/PluginProcessor.cpp
	src/PluginProcessor.h
        src/CabSimulation.h
        src/Equalizer.cpp
        src/Equalizer.h
	src/RTNeuralLSTM.cpp
        src/RTNeuralLSTM.h
)

juce_add_binary_data(BinaryData SOURCES

    #Add all the json models
    train/models/gainStable/acousticModelGainStable.json
    train/models/gainStable/blackPanelModelGainStable.json
    train/models/gainStable/BritComboModelGainStable.json
    train/models/gainStable/classicModelGainStable.json
    train/models/gainStable/dynamicAmpModelGainStable.json
    train/models/gainStable/extremeModelGainStable.json
    train/models/gainStable/metalModelGainStable.json
    train/models/gainStable/rFierModelGainStable.json
    train/models/gainStable/tweedModelGainStable.json
	
    train/models/parametrizedGain/acousticModelParametrizedGain.json
    train/models/parametrizedGain/blackPanelModelParametrizedGain.json
    train/models/parametrizedGain/britComboModelParametrizedGain.json
    train/models/parametrizedGain/classicModelParametrizedGain.json
    train/models/parametrizedGain/dynamicAmpModelParametrizedGain.json
    train/models/parametrizedGain/extremeModelParametrizedGain.json
    train/models/parametrizedGain/metalModelParametrizedGain.json
    train/models/parametrizedGain/rFierModelParemtrizedGain.json
    train/models/parametrizedGain/tweedModelParametrizedGain.json

    #resources/images/cube-40xl_top_gal.jpg #Aggiungi tutte le immagini per la GUI
    resources/background_on.jpg
    resources/background_on_blue.jpg
    resources/default_ir.wav
    resources/cab_switch_off.png
    resources/cab_switch_on.png
    resources/big_knob.png
    resources/small_knob.png
)

target_compile_definitions(RolandCube
    PUBLIC # 
        JUCE_DISPLAY_SPLASH_SCREEN=0
        JUCE_REPORT_APP_USAGE=0
        JUCE_ALSA=1
        JUCE_DIRECTSOUND=1
        JUCE_DISABLE_CAUTIOUS_PARAMETER_ID_CHECKING=1
        JUCE_USE_OGGVORBIS=1
        JUCE_WEB_BROWSER=0  
        JUCE_USE_CURL=0     
        JUCE_VST3_CAN_REPLACE_VST2=0)


target_link_libraries(RolandCube 
    PRIVATE
	juce::juce_audio_utils
        juce::juce_dsp
	
    PUBLIC
	BinaryData
	RTNeural
	chowdsp_dsp
	nlohmann_json::nlohmann_json
        juce::juce_audio_plugin_client
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)