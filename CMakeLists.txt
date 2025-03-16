cmake_minimum_required(VERSION 3.5.0)
set(CMAKE_CXX_STANDARD 20)
project(c_audio_playground VERSION 0.1.0 LANGUAGES C CXX)

# 2/  Option 2: simply fetch hello_imgui during the build
if (NOT TARGET hello_imgui)
    message(STATUS "Fetching hello_imgui")
    include(FetchContent)
    FetchContent_Declare(hello_imgui GIT_REPOSITORY https://github.com/pthom/hello_imgui.git GIT_TAG master)
    FetchContent_MakeAvailable(hello_imgui)
endif()

hello_imgui_add_app(c_audio_playground ${CMAKE_SOURCE_DIR}/src/main.cpp)

# add_executable(c_audio_playground ${CMAKE_SOURCE_DIR}/src/main.cpp)


set(ALLOW_DUPLICATE_CUSTOM_TARGETS TRUE)

# Add subdirectories
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/src)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/core)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/library)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/nodes)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/components)

# Port Audio
add_subdirectory(${CMAKE_SOURCE_DIR}/lib/portaudio)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/lib/portaudio/include)
target_link_libraries(${PROJECT_NAME} PUBLIC ${CMAKE_BINARY_DIR}/lib/portaudio/libportaudio.a)

# RT Midi
add_subdirectory(${CMAKE_SOURCE_DIR}/lib/rtmidi)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/lib/rtmidi)
target_link_directories(${PROJECT_NAME} PUBLIC ${CMAKE_BINARY_DIR}/lib/rtmidi)
target_link_libraries(${PROJECT_NAME} PUBLIC ${CMAKE_BINARY_DIR}/lib/rtmidi/librtmidi.dylib)

# Choc
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/lib/choc)

# Moody concurrentqueue
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/lib/concurrentqueue)

# Brickworks
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/lib/brickworks/include)

#stduuid
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/lib/stduuid/include)

# DSP-CPP-filters
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/lib/dsp-cpp-filters/lib)

# RCH Oscillators
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/lib/rchoscillators/library)

# Pybind11
add_subdirectory(${CMAKE_SOURCE_DIR}/lib/pybind11)
target_link_libraries(${PROJECT_NAME} PRIVATE pybind11::embed)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/lib/pybind11/include)


FetchContent_Declare(
  yaml-cpp
  GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
  GIT_TAG master # Can be a tag (yaml-cpp-x.x.x), a commit hash, or a branch name (master)
)
FetchContent_MakeAvailable(yaml-cpp)


# yaml-cpp
target_link_libraries(${PROJECT_NAME} PUBLIC yaml-cpp::yaml-cpp) # The library or executable that require yaml-cpp library

# Lua integration
add_subdirectory(${CMAKE_SOURCE_DIR}/lib/lua-cmake)
target_link_libraries(${PROJECT_NAME} PUBLIC lua::lib)

add_subdirectory(${CMAKE_SOURCE_DIR}/lib/sol2)
target_link_libraries(${PROJECT_NAME} PUBLIC sol2::sol2)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/lib/sol2/include)


# Im Gui
target_link_libraries(${PROJECT_NAME} PRIVATE hello_imgui)
target_include_directories(${PROJECT_NAME} PRIVATE ${hello_imgui_SOURCE_DIR}/include)



# Midi Scripts copy commands
set(MIDI_SCRIPT_SRC_DIR ${CMAKE_SOURCE_DIR}/src/control_scripts)
set(MIDI_SCRIPTS_APPLE_DST_DIR ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.app/Contents/Resources/control_scripts)





# Add the necessary frameworks for macOS
if(APPLE)
  target_link_libraries(${PROJECT_NAME} PRIVATE
  "-framework CoreMidi"
  "-framework Cocoa"
  "-framework Accelerate"
  "-framework AudioToolbox"
  "-framework AudioUnit"
  "-framework CoreServices"
  "-framework CoreAudio"
  "-framework Carbon"
  "-framework CoreFoundation"
    "-framework IOKit"
    "-framework WebKit"
    "-framework OpenGL"
  )

  add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${MIDI_SCRIPT_SRC_DIR} ${MIDI_SCRIPTS_APPLE_DST_DIR}
    DEPENDS ${MIDI_SCRIPT_SRC_DIR}
    VERBATIM
)
  
endif()