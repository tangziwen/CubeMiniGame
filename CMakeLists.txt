cmake_minimum_required(VERSION 3.12)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Build type" FORCE)
endif()

project(CubeEngine)

# ------------------------------------------------------------------------------
# Third-party include paths.
# The old CMake used global include_directories(), so every source file inherited
# these paths. Keep the list explicit and attach it to the targets that compile
# or expose third-party headers.
# ------------------------------------------------------------------------------
set(CUBEENGINE_EXTERNAL_INCLUDE_DIRS
    # Some bundled third-party sources are customized to include engine headers,
    # for example NodeEditor includes "2D/imgui.h".
    ${CMAKE_CURRENT_SOURCE_DIR}/CubeEngine/EngineSrc
    ${CMAKE_CURRENT_SOURCE_DIR}/external
    ${CMAKE_CURRENT_SOURCE_DIR}/external/FastNoise
    ${CMAKE_CURRENT_SOURCE_DIR}/external/FreeType
    ${CMAKE_CURRENT_SOURCE_DIR}/external/GLFW3/include
    ${CMAKE_CURRENT_SOURCE_DIR}/external/GLEW
    ${CMAKE_CURRENT_SOURCE_DIR}/external/vulkan
    ${CMAKE_CURRENT_SOURCE_DIR}/external/spirv_cross
    ${CMAKE_CURRENT_SOURCE_DIR}/external/glslang
    ${CMAKE_CURRENT_SOURCE_DIR}/external/gli
    ${CMAKE_CURRENT_SOURCE_DIR}/external/glm
    ${CMAKE_CURRENT_SOURCE_DIR}/external/rapidjson/include
    ${CMAKE_CURRENT_SOURCE_DIR}/external/Python/include
    ${CMAKE_CURRENT_SOURCE_DIR}/external/pybind11
    ${CMAKE_CURRENT_SOURCE_DIR}/external/SoLoud/include
    ${CMAKE_CURRENT_SOURCE_DIR}/external/box2d
    ${CMAKE_CURRENT_SOURCE_DIR}/external/Bullet
    ${CMAKE_CURRENT_SOURCE_DIR}/external/Lua
    ${CMAKE_CURRENT_SOURCE_DIR}/external/LuaBridge
    ${CMAKE_CURRENT_SOURCE_DIR}/external/NodeEditor/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/external/NodeEditor/Source
    ${CMAKE_CURRENT_SOURCE_DIR}/external/NodeEditor/BlueprintUtilities/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/external/noise
    ${CMAKE_CURRENT_SOURCE_DIR}/external/SOIL2
    ${CMAKE_CURRENT_SOURCE_DIR}/external/zip
)

# ------------------------------------------------------------------------------
# External library search paths (kept for the prebuilt libs used by the project)
# Must be set before defining the executable target.
# ------------------------------------------------------------------------------
link_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/CubeEngine/libVC
    ${CMAKE_CURRENT_SOURCE_DIR}/external
    ${CMAKE_CURRENT_SOURCE_DIR}/external/Bullet
)

# ------------------------------------------------------------------------------
# External third-party sources (compiled from source, like the old setup)
# We use GLOB here because these are third-party files we do not maintain.
# ------------------------------------------------------------------------------
file(GLOB_RECURSE EXTERNAL_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/external/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/external/*.c
)

# Do not blindly compile all Bullet sources: Bullet3OpenCL needs external OpenCL
# headers. Build the Bullet2 modules used by the engine from source so they stay
# ABI-compatible with the checked-in Bullet headers.
list(FILTER EXTERNAL_SOURCES EXCLUDE REGEX "/Bullet/.*")
file(GLOB_RECURSE BULLET_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/external/Bullet/LinearMath/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/external/Bullet/BulletCollision/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/external/Bullet/BulletDynamics/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/external/Bullet/BulletSoftBody/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/external/Bullet/BulletInverseDynamics/*.cpp
)
list(APPEND EXTERNAL_SOURCES ${BULLET_SOURCES})

add_library(CubeEngineExternal STATIC ${EXTERNAL_SOURCES})

target_compile_features(CubeEngineExternal PUBLIC cxx_std_17)

target_include_directories(CubeEngineExternal
    PUBLIC
        ${CUBEENGINE_EXTERNAL_INCLUDE_DIRS}
)

target_compile_definitions(CubeEngineExternal
    PUBLIC
        GLEW_STATIC
)

if(MSVC)
    target_compile_options(CubeEngineExternal PRIVATE /utf-8)
endif()

# ------------------------------------------------------------------------------
# Engine and Application are built as separate static libraries.
# Each subdirectory lists its own source files explicitly.
# ------------------------------------------------------------------------------
add_subdirectory(CubeEngine/EngineSrc)
add_subdirectory(CubeEngine/Application)

# ------------------------------------------------------------------------------
# Main executable
# ------------------------------------------------------------------------------
add_executable(${CMAKE_PROJECT_NAME} CubeEngine/main.cpp)

target_compile_features(${CMAKE_PROJECT_NAME} PRIVATE cxx_std_17)

target_include_directories(${CMAKE_PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/CubeEngine
)

# ------------------------------------------------------------------------------
# Link engine + application + external sources; prebuilt libs are linked here
# ------------------------------------------------------------------------------
target_link_libraries(${CMAKE_PROJECT_NAME}
    PRIVATE
        CubeEngineApp
        CubeEngineEngine
        CubeEngineExternal
        freetype
        glfw3
        glew32s
        vulkan-1
        spirv-cross-c
        spirv-cross-core
        spirv-cross-cpp
        spirv-cross-glsl
        spirv-cross-reflect
        spirv-cross-util
        spirv-cross-c-shared
        spirv-cross-hlsl
        spirv-cross-msl
        glslang
        shaderc_shared
        python314
        fmodex_vc
        opengl32
        Dbghelp
        soloud_static
        box2d
        zlib
        libpng
)

# ------------------------------------------------------------------------------
# Compiler options
# ------------------------------------------------------------------------------
if(MSVC)
    target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE /utf-8)
endif()
