cmake_minimum_required(VERSION 3.15)

set(PROJECT_NAME "testexplore")
set(PROJECT_DESCRIPTION "Test JUCE")

project("${PROJECT_NAME}" DESCRIPTION "${PROJECT_DESCRIPTION}" VERSION 1.0)

include("cmake/CPM.cmake")

# If the generator supports it you will get files organised in folders in the IDE
set_property(GLOBAL PROPERTY USE_FOLDERS YES)

# Create a /Modules directory in the IDE with the JUCE Module code
option(JUCE_ENABLE_MODULE_SOURCE_GROUPS "Show all module sources in IDE projects" ON)

CPMAddPackage(
  NAME JUCE
  GITHUB_REPOSITORY juce-framework/JUCE
  VERSION 06325689dd3661e5bd35a2906cbeb4ee15a3f91f
  GIT_TAG 06325689dd3661e5bd35a2906cbeb4ee15a3f91f
)

set(BUNDLE_ID com.3dengineer.${PROJECT_NAME})

# This second plugin is an AUv3 plugin of type kAudioUnitType_MusicEffect
set(PROJECT_NAME_AUMF ${PROJECT_NAME}_aumf)
set(BUNDLE_ID_AUMF com.3dengineer.${PROJECT_NAME}.${PROJECT_NAME}-AUv3-aumf)

set(ICLOUD_PERMISSIONS_ENABLED TRUE)
file(READ "Info.plist" PLIST_CONTENT_TO_MERGE)

file(READ "bundleid_info.plist" AUMF_INFO_PLIST)

# Replace occurrences of "BUNDLE_ID_HERE" in the input string with the value of the variable BUNDLE_ID_AUMF
string(REPLACE "BUNDLE_ID_HERE" ${BUNDLE_ID_AUMF} AUMF_INFO_PLIST ${AUMF_INFO_PLIST})

juce_add_plugin(${PROJECT_NAME}
    COMPANY_NAME "James Swift"                  # Specify the name of the plugin's author
    BUNDLE_ID ${BUNDLE_ID}                      # Bundle ID for Apple
    IS_SYNTH FALSE                              # Is this a synth or an effect?
    NEEDS_MIDI_INPUT FALSE                      # Does the plugin need midi input?
    NEEDS_MIDI_OUTPUT FALSE                     # Does the plugin need midi output?
    IS_MIDI_EFFECT FALSE                        # Is this plugin a MIDI effect?
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE           # Does the editor need keyboard focus?
    COPY_PLUGIN_AFTER_BUILD TRUE                # Should the plugin be installed to a default location after building?
    PLUGIN_MANUFACTURER_CODE Js3d               # A four-character manufacturer id with at least one upper-case character
    PLUGIN_CODE jsIs                            # A unique four-character plugin id with at least one upper-case character
    FORMATS AUv3 Standalone                     # The formats to build. Other valid formats are: AAX Unity VST AU AUv3
    ICLOUD_PERMISSIONS_ENABLED ${ICLOUD_PERMISSIONS_ENABLED} # Enable iCloud permissions
    PLIST_CONTENT_TO_MERGE ${PLIST_CONTENT_TO_MERGE} # Merge the contents of this plist file into the Info.plist
    PLUGIN_NAME "${PROJECT_NAME} A"              # The plugin name displayed in the DAW.
    PRODUCT_NAME ${PROJECT_NAME}                # The name of the final executable, which can differ from the target name
    DESCRIPTION ${PROJECT_DESCRIPTION}          # A short description of the plugin.
    MICROPHONE_PERMISSION_ENABLED TRUE
    MICROPHONE_PERMISSION_TEXT "This app requires access to your microphone to function."
    PLIST_TO_MERGE "${PLIST_CONTENT_TO_MERGE}"
    REQUIRES_FULL_SCREEN TRUE
    IPHONE_SCREEN_ORIENTATIONS UIInterfaceOrientationLandscapeLeft UIInterfaceOrientationLandscapeRight
    IPAD_SCREEN_ORIENTATIONS UIInterfaceOrientationLandscapeLeft UIInterfaceOrientationLandscapeRight
)

juce_add_plugin(${PROJECT_NAME_AUMF}
    COMPANY_NAME "James Swift"                  
    IS_SYNTH FALSE                              
    BUNDLE_ID ${BUNDLE_ID_AUMF}                 # Different bundle ID
    NEEDS_MIDI_INPUT FALSE                      
    NEEDS_MIDI_OUTPUT FALSE                     
    IS_MIDI_EFFECT FALSE                        
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE           
    COPY_PLUGIN_AFTER_BUILD TRUE                
    PLUGIN_MANUFACTURER_CODE Js3d               
    PLUGIN_CODE jsIm                            # Different plugin code
    FORMATS AUv3                                
    AU_MAIN_TYPE kAudioUnitType_MusicEffect     # This is different from the other plugin and the reason we want to deliver two plugins
    ICLOUD_PERMISSIONS_ENABLED ${ICLOUD_PERMISSIONS_ENABLED}
    PLIST_CONTENT_TO_MERGE ${AUMF_INFO_PLIST}   # Different plist content
    PLUGIN_NAME "${PROJECT_NAME} B"             # The plugin name displayed in the DAW.
    PRODUCT_NAME ${PROJECT_NAME_AUMF}           # Different product name
    DESCRIPTION ${PROJECT_DESCRIPTION}
    MICROPHONE_PERMISSION_ENABLED TRUE
    MICROPHONE_PERMISSION_TEXT "This app requires access to your microphone to function."
    REQUIRES_FULL_SCREEN TRUE
    IPHONE_SCREEN_ORIENTATIONS UIInterfaceOrientationLandscapeLeft UIInterfaceOrientationLandscapeRight
    IPAD_SCREEN_ORIENTATIONS UIInterfaceOrientationLandscapeLeft UIInterfaceOrientationLandscapeRight
)

add_dependencies(${PROJECT_NAME}_Standalone ${PROJECT_NAME_AUMF}_AUv3)

set_target_properties(${PROJECT_NAME_AUMF} ${PROJECT_NAME_AUMF}_AUv3 PROPERTIES
  XCODE_ATTRIBUTE_SKIP_INSTALL "YES"
)

# This will copy the second AUv3 appex to the Standalone plugin
add_custom_command(TARGET "${PROJECT_NAME}_Standalone" POST_BUILD
  COMMAND "${CMAKE_COMMAND}"
      "-Dsrc=$<GENEX_EVAL:$<TARGET_PROPERTY:${PROJECT_NAME_AUMF},RUNTIME_OUTPUT_DIRECTORY>>/AUv3/${PROJECT_NAME_AUMF}.appex"
      "-Ddest=$<GENEX_EVAL:$<TARGET_PROPERTY:${PROJECT_NAME},RUNTIME_OUTPUT_DIRECTORY>>/Standalone/${PROJECT_NAME}.app/PlugIns/"
      "-P" "${JUCE_CMAKE_UTILS_DIR}/copyDir.cmake"
  VERBATIM
)

# `juce_generate_juce_header` will create a JuceHeader.h for a given target, which will be generated
# into your build tree. This should be included with `#include <JuceHeader.h>`. The include path for
# this header will be automatically added to the target. The main function of the JuceHeader is to
# include all your JUCE module headers; if you're happy to include module headers directly, you
# probably don't need to call this.

juce_generate_juce_header(${PROJECT_NAME})
juce_generate_juce_header(${PROJECT_NAME_AUMF})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

if(MSVC)
  message(STATUS "Static Runtime Library")
  target_compile_options(${PROJECT_NAME} PUBLIC
    $<$<CONFIG:Release>:/MT>        # Runtime library: Multi-threaded
    $<$<CONFIG:MinSizeRel>:/MT>     # Runtime library: Multi-threaded
    $<$<CONFIG:RelWithDebInfo>:/MT> # Runtime library: Multi-threaded
    $<$<CONFIG:Debug>:/MTd>         # Runtime library: Multi-threaded Debug
  )
else()
  set(CMAKE_OSX_DEPLOYMENT_TARGET "12.0" CACHE STRING "Minimum OS X deployment version" FORCE)
  message(STATUS "Minimum OS X deployment version " ${CMAKE_OSX_DEPLOYMENT_TARGET})
endif()

# Plugin Files
FILE(GLOB CPP_FILES src/*.cpp)
FILE(GLOB HPP_FILES Source/*.h)

target_sources(${PROJECT_NAME}
    PRIVATE
    ${CPP_FILES}
    ${HPP_FILES})

target_compile_definitions(${PROJECT_NAME}
    PUBLIC
        # JUCE_WEB_BROWSER and JUCE_USE_CURL would be on by default, but you might not need them.
        JUCE_WEB_BROWSER=0  # If you remove this, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_plugin` call
        JUCE_USE_CURL=0     # If you remove this, add `NEEDS_CURL TRUE` to the `juce_add_plugin` call
        JUCE_VST3_CAN_REPLACE_VST2=0
        DONT_SET_USING_JUCE_NAMESPACE=1
    )

# Need to add the sources to the second plugin target
target_sources(${PROJECT_NAME_AUMF}
    PRIVATE
    ${CPP_FILES}
    ${HPP_FILES})

set(COMMON_LIBS
  juce::juce_audio_utils
  juce::juce_dsp
  juce::juce_osc
  juce::juce_opengl
)

set(COMMON_JUCE_LINK_FLAGS
  juce::juce_recommended_config_flags
  juce::juce_recommended_lto_flags
  juce::juce_recommended_warning_flags
)

target_link_libraries(${PROJECT_NAME}
  PRIVATE
      ${COMMON_LIBS}
  PUBLIC
      ${COMMON_JUCE_LINK_FLAGS}
)

# Need to link the second plugin target to the same libraries
target_link_libraries(${PROJECT_NAME}_aumf
  PRIVATE
      ${COMMON_LIBS}
  PUBLIC
      ${COMMON_JUCE_LINK_FLAGS}
)
