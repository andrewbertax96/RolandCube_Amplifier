cmake_minimum_required(VERSION 3.15)
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.12" CACHE STRING "Minimum OS X deployment target")  

project(RolandCube VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 17)  #aggiunto
add_subdirectory(modules/JUCE ./JUCE)

set(RTNEURAL_STL ON CACHE BOOL "Use RTNeural with XSMID backend" FORCE)
add_subdirectory(modules/RTNeural ./RTNeural)
include_directories(modules/RTNeural ./RTNeural)
                   
juce_add_plugin(RolandCube
    COMPANY_NAME Roland                          
    COPY_PLUGIN_AFTER_BUILD TRUE       
    PLUGIN_MANUFACTURER_CODE Bertazzi               
    PLUGIN_CODE RoCu                            
    
    VST3_CATEGORIES Fx Distortion                                            
    FORMATS AU VST3 Standalone                  
    PRODUCT_NAME "RolandCube")        

juce_generate_juce_header(RolandCube)

target_sources(RolandCube
    PRIVATE
    src/PluginEditor.cpp
    src/PluginProcessor.cpp
    src/PluginEditor.h
    src/PluginProcessor.h)

juce_add_binary_data(BinaryData SOURCES
    #ModelTrain/neural_net_weights.json #Inserisci il modello della rete neurale dalla corretta directory del train
    resources/images/cube-40xl_top_gal.jpg #Aggiungi tutte le immagini per la GUI
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


target_link_libraries(RolandCube PUBLIC
	BinaryData
	RTNeural
	juce::juce_audio_utils
        juce::juce_dsp
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)