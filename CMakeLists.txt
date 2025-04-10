cmake_minimum_required(VERSION 3.15)

set(PROJECT_NAME "testandexplore")
set(PROJECT_DESCRIPTION "Test JUCE")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(${PROJECT_NAME} DESCRIPTION "${PROJECT_DESCRIPTION}" VERSION 1.0)

include("cmake/CPM.cmake")

# If the generator supports it you will get files organised in folders in the IDE
set_property(GLOBAL PROPERTY USE_FOLDERS YES)

# Create a /Modules directory in the IDE with the JUCE Module code
option(JUCE_ENABLE_MODULE_SOURCE_GROUPS "Show all module sources in IDE projects" ON)

# Build for iOS?
option(BUILD_IOS "Build for iOS" OFF)

CPMAddPackage(
  NAME JUCE
  GITHUB_REPOSITORY juce-framework/JUCE
  GIT_TAG 8.0.7
)

# If you are building a VST2 or AAX plugin, CMake needs to be told where to find these SDKs on your
# system. This setup should be done before calling `juce_add_plugin`.

# juce_set_vst2_sdk_path(...)
# juce_set_aax_sdk_path(...)

# `juce_add_plugin` adds a static library target with the name passed as the first argument
# This target is a normal CMake target, but has a lot of extra properties set
# up by default. As well as this shared code static library, this function adds targets for each of
# the formats specified by the FORMATS arguments. This function accepts many optional arguments.
# Check the readme at `docs/CMake API.md` in the JUCE repo for the full list.

# If building for iOS set FORMATS to AUv3 and Standalone
if(BUILD_IOS)
  set(FORMATS AUv3 Standalone)
  set(ICLOUD_PERMISSIONS_ENABLED TRUE)
  file(READ "Info.plist" PLIST_CONTENT_TO_MERGE)
else()
  set(FORMATS AU VST3)
  set(ICLOUD_PERMISSIONS_ENABLED FALSE)
endif()

juce_add_plugin(${PROJECT_NAME}
  COMPANY_NAME "James Swift" # Specify the name of the plugin's author
  BUNDLE_ID com.3dengineer.testexplore # Bundle ID for Apple
  IS_SYNTH FALSE # Is this a synth or an effect?
  NEEDS_MIDI_INPUT TRUE # Does the plugin need midi input?
  NEEDS_MIDI_OUTPUT FALSE # Does the plugin need midi output?
  IS_MIDI_EFFECT FALSE # Is this plugin a MIDI effect?
  EDITOR_WANTS_KEYBOARD_FOCUS FALSE # Does the editor need keyboard focus?
  COPY_PLUGIN_AFTER_BUILD TRUE # Should the plugin be installed to a default location after building?
  PLUGIN_MANUFACTURER_CODE Js3d # A four-character manufacturer id with at least one upper-case character
  PLUGIN_CODE jsIs # A unique four-character plugin id with at least one upper-case character
  FORMATS ${FORMATS} # The formats to build. Other valid formats are: AAX Unity VST AU AUv3
  ICLOUD_PERMISSIONS_ENABLED ${ICLOUD_PERMISSIONS_ENABLED} # Enable iCloud permissions
  PLIST_CONTENT_TO_MERGE ${PLIST_CONTENT_TO_MERGE} # Merge the contents of this plist file into the Info.plist
  PLUGIN_NAME ${PROJECT_NAME} # The plugin name displayed in the DAW.
  PRODUCT_NAME ${PROJECT_NAME} # The name of the final executable, which can differ from the target name
  DESCRIPTION ${PROJECT_DESCRIPTION} # A short description of the plugin.
  AAX_CATEGORY AAX_EPlugInCategory_Effect
  AU_MAIN_TYPE kAudioUnitType_MusicEffect
  VST3_CATEGORIES Fx
  MICROPHONE_PERMISSION_ENABLED TRUE
  MICROPHONE_PERMISSION_TEXT "This app requires access to your microphone to function."
  PLIST_TO_MERGE "${PLIST_CONTENT_TO_MERGE}"
  REQUIRES_FULL_SCREEN TRUE
  IPHONE_SCREEN_ORIENTATIONS UIInterfaceOrientationLandscapeLeft UIInterfaceOrientationLandscapeRight
  IPAD_SCREEN_ORIENTATIONS UIInterfaceOrientationLandscapeLeft UIInterfaceOrientationLandscapeRight
)

# `juce_generate_juce_header` will create a JuceHeader.h for a given target, which will be generated
# into your build tree. This should be included with `#include <JuceHeader.h>`. The include path for
# this header will be automatically added to the target. The main function of the JuceHeader is to
# include all your JUCE module headers; if you're happy to include module headers directly, you
# probably don't need to call this.

juce_generate_juce_header(${PROJECT_NAME})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 23)

get_property(CXX_STANDARD_VALUE TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD)
message(STATUS "C++ Standard: " ${CXX_STANDARD_VALUE})

set(COMPILER_WARNINGS
  # Maximum warnings (or very nearly)
  $<$<CXX_COMPILER_ID:MSVC>:/${GSDSP_MSVC_WARN_LEVEL}>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -pedantic>

  # Warnings as errors but only for release builds
  $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Release>>:/WX>
  $<$<AND:$<NOT:$<CXX_COMPILER_ID:MSVC>>,$<CONFIG:Release>>:-Werror>

  # Disable PGO unprofiled warning when using profiles
  $<$<AND:$<CXX_COMPILER_ID:Clang>,$<BOOL:${GSDSP_ENABLE_PGO_USE}>>:-Wno-error=profile-instr-unprofiled>

  # If we're using gcc turn of ABI warnings with no-psabi
  $<$<CXX_COMPILER_ID:GNU>:-Wno-psabi>

  # C++20 introduces the char8_t type for UTF-8 but JUCE isn't there yet so disable that type
  $<$<CXX_COMPILER_ID:MSVC>:/Zc:char8_t->
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-fno-char8_t>

  # macOS/Linux warnings
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wno-deprecated-anon-enum-enum-conversion
  -Wno-c++98-compat-extra-semi
  -Wno-float-equal
  -Wno-implicit-float-conversion
  $<$<AND:$<CXX_COMPILER_ID:Clang>,$<VERSION_GREATER_EQUAL:$<CXX_COMPILER_VERSION>,20.0>>:-Wno-nontrivial-memcall>>

  # Just My Code debugging for VS
  $<$<CXX_COMPILER_ID:MSVC>:/JMC>
)

if(MSVC)
  message(STATUS "Static Runtime Library")
  target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:Release>:/MT> # Runtime library: Multi-threaded
    $<$<CONFIG:MinSizeRel>:/MT> # Runtime library: Multi-threaded
    $<$<CONFIG:RelWithDebInfo>:/MT> # Runtime library: Multi-threaded
    $<$<CONFIG:Debug>:/MTd> # Runtime library: Multi-threaded Debug
  )
else()
  set(CMAKE_OSX_DEPLOYMENT_TARGET "10.13" CACHE STRING "Minimum OS X deployment version" FORCE)
  message(STATUS "Minimum OS X deployment version " ${CMAKE_OSX_DEPLOYMENT_TARGET})
  target_compile_options(${PROJECT_NAME} PRIVATE ${COMPILER_WARNINGS})
endif()

# Plugin Files
FILE(GLOB CPP_FILES src/*.cpp)
FILE(GLOB HPP_FILES Source/*.h)

target_sources(${PROJECT_NAME}
  PRIVATE
  ${CPP_FILES}
  ${HPP_FILES})

target_compile_definitions(${PROJECT_NAME}
  PRIVATE
  # JUCE_WEB_BROWSER and JUCE_USE_CURL would be on by default, but you might not need them.
  JUCE_WEB_BROWSER=0 # If you remove this, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_plugin` call
  JUCE_USE_CURL=0 # If you remove this, add `NEEDS_CURL TRUE` to the `juce_add_plugin` call
  JUCE_VST3_CAN_REPLACE_VST2=0
  DONT_SET_USING_JUCE_NAMESPACE=1
  JUCE_USE_CAMERA=1
)

set(COMMON_LIBS
  juce::juce_audio_utils
  juce::juce_dsp
  juce::juce_osc
  juce::juce_video
)

set(COMMON_JUCE_LINK_FLAGS
  juce::juce_recommended_config_flags
  juce::juce_recommended_lto_flags
)

target_link_libraries(${PROJECT_NAME}
  PRIVATE
  ${COMMON_LIBS}
  PRIVATE
  ${COMMON_JUCE_LINK_FLAGS}
)

