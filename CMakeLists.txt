cmake_minimum_required(VERSION 3.12)

if(POLICY CMP0091)
    cmake_policy(SET CMP0091 NEW)
endif()

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Build type")
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
        Debug Release RelWithDebInfo MinSizeRel)
endif()

project(CubeMiniGame)

set(CUBE_CLIENT_TARGET CubeClient)

option(CUBEENGINE_USE_VULKAN "Build Vulkan backend" ON)
option(CUBEENGINE_USE_OPENGL "Build OpenGL backend" ON)
option(CUBEENGINE_USE_PYTHON_SCRIPTING "Build Python scripting support" ON)
option(CUBEENGINE_USE_FMOD "Build FMOD audio support" ON)
option(CUBEENGINE_USE_SOLOUD "Build SoLoud audio support" ON)
option(CUBEENGINE_BUILD_BULLET_FROM_SOURCE "Build Bullet from checked-in source" ON)

include(cmake/PrebuiltTargets.cmake)
include(cmake/ThirdPartyTargets.cmake)

# ------------------------------------------------------------------------------
# Engine and Application are built as separate static libraries.
# Each subdirectory lists its own source files explicitly.
# Target names:
#   CubeEngine - engine static library
#   CubeGame   - application/game static library
#   CubeClient - final executable
# ------------------------------------------------------------------------------
add_subdirectory(CubeEngine/EngineSrc)
add_subdirectory(CubeEngine/Application)

# ------------------------------------------------------------------------------
# Main executable
# ------------------------------------------------------------------------------
add_executable(${CUBE_CLIENT_TARGET} CubeEngine/main.cpp)

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT ${CUBE_CLIENT_TARGET})

target_compile_features(${CUBE_CLIENT_TARGET} PRIVATE cxx_std_17)

target_include_directories(${CUBE_CLIENT_TARGET}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/CubeEngine
)

# ------------------------------------------------------------------------------
# The executable only depends on the application target; engine and third-party
# dependencies are expressed by their owning targets.
# ------------------------------------------------------------------------------
target_link_libraries(${CUBE_CLIENT_TARGET}
    PRIVATE
        CubeGame
)

# ------------------------------------------------------------------------------
# Compiler options
# ------------------------------------------------------------------------------
if(MSVC)
    target_compile_options(${CUBE_CLIENT_TARGET} PRIVATE /utf-8)
endif()
