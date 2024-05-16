cmake_minimum_required(VERSION 3.15)
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.12" CACHE STRING "Minimum OS X deployment target")  
project(RolandCube VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(modules)
include_directories(modules)

#juce_set_aax_sdk_path(C:/SDKs/AAX_SDK/)

set(JUCE_FORMATS AU VST3 Standalone)

juce_add_plugin(RolandCube
    COMPANY_NAME Roland                          
    PLUGIN_MANUFACTURER_CODE Brtx               
    PLUGIN_CODE RoCu                            
    FORMATS ${JUCE_FORMATS}  
    ProductName "RolandCube"

    VST3_CATEGORIES Fx Distortion

    MICROPHONE_PERMISSION_ENABLED TRUE            
    
)

# create JUCE header
juce_generate_juce_header(RolandCube)

# add sources
add_subdirectory(src)
include_directories(src)
add_subdirectory(resources)

target_compile_definitions(RolandCube
    PUBLIC
    JUCE_DISPLAY_SPLASH_SCREEN=0
    JUCE_REPORT_APP_USAGE=0
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_MODAL_LOOPS_PERMITTED=1
)

target_link_libraries(RolandCube PUBLIC
    juce_plugin_modules
)

