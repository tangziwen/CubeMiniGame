cmake_minimum_required(VERSION 3.5)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Build type" FORCE)
endif()

#predefined variables
set(SOURCES "")
set(HEADERS "")
set(RESOURCES "")
set(FORMS "")
set(INCLUDEPATH "")
set(LIBS "")
set(QMAKE_LFLAGS "")
set(CMAKE_CXX_STANDARD 17)
set(PWD "E:/Cube-Engine/CubeEngine")
#-------------------------------------------------
#-------------------------------------------------
#
#
# Project created by QtCreator 2015-08-27T16:28:01
# Project created by QtCreator 2015-08-27T16:28:01
#
#
#-------------------------------------------------
#-------------------------------------------------
project(CubeEngine)
#CONFIG += console
#CONFIG += console
set(TEMPLATE app)
#OpenGL
#OpenGL
###Application folder BEGIN###
include_directories(./CubeEngine)
include_directories(./CubeEngine/Application)
###Application folder END###

####External library BEGIN####
include_directories(./external)
#FreeType#
set(LIBS ${LIBS} freetype)
include_directories(./external/FreeType)
#GLFW 3.0#
set(LIBS ${LIBS} glfw3)
include_directories(./external/GLFW3/include)
#GLEW #
set(LIBS ${LIBS} glew32s)
include_directories(./external/GLEW)

#Vulkan
set(LIBS ${LIBS} VkLayer_utils)
set(LIBS ${LIBS} vulkan-1)
include_directories(./external/vulkan/)
#spirv cross
set(LIBS ${LIBS} spirv-cross-c)
set(LIBS ${LIBS} spirv-cross-core)
set(LIBS ${LIBS} spirv-cross-cpp)
set(LIBS ${LIBS} spirv-cross-glsl)
set(LIBS ${LIBS} spirv-cross-reflect)
set(LIBS ${LIBS} spirv-cross-util)
set(LIBS ${LIBS} spirv-cross-c-shared)
set(LIBS ${LIBS} spirv-cross-hlsl)
set(LIBS ${LIBS} spirv-cross-msl)
include_directories(./external/spirv_cross/)
#glslang
set(LIBS ${LIBS} glslang)
include_directories(./external/glslang/)

#shaderc
set(LIBS ${LIBS} shaderc_shared)

#gli
include_directories(./external/gli/)

add_compile_definitions(WITH_OPENCV2)

#rapidJson
include_directories(./external/rapidjson/include)

#python
include_directories(./external/Python/include)
set(LIBS ${LIBS} python314)

#FMOD
set(LIBS ${LIBS} fmodex_vc)

#GL
set(LIBS ${LIBS} opengl32)

#debugHelp
set(LIBS ${LIBS} Dbghelp)

#SoLoud
set(LIBS ${LIBS} soloud_static)
include_directories(./external/Soloud)

#box2d
set(LIBS ${LIBS} box2d)
include_directories(./external/box2d)

#bullets
#set(LIBS ${LIBS} Bullet3Collision_vs2010_x64_debug Bullet3Common_vs2010_x64_debug Bullet3Dynamics_vs2010_x64_debug Bullet3Geometry_vs2010_x64_debug Bullet3OpenCL_clew_vs2010_x64_debug)
#set(LIBS ${LIBS} BulletCollision_vs2010_x64_debug BulletDynamics_vs2010_x64_debug BulletInverseDynamics_vs2010_x64_debug BulletInverseDynamicsUtils_vs2010_x64_debug BulletSoftBody_vs2010_x64_debug)
#set(LIBS ${LIBS} BussIK_vs2010_x64_debug gwen_vs2010_x64_debug LinearMath_vs2010_x64_debug)
include_directories(./external/Bullet)


#misc
set(LIBS ${LIBS} zlib libpng )


####External library END####

set(_src_root_path "${CMAKE_CURRENT_SOURCE_DIR}/CubeEngine")

#SIMD
add_compile_options("/arch:AVX")



set(LINK_DIR ./CubeEngine/libVC)
link_directories(${LINK_DIR})
link_directories(./external)
link_directories(./external/Bullet)


macro(source_group_by_dir source_files)
    set(sgbd_cur_dir ${_src_root_path})
    foreach(sgbd_file ${${source_files}})
        string(REGEX REPLACE ${sgbd_cur_dir}/\(.*\) \\1 sgbd_fpath ${sgbd_file})
        string(REGEX REPLACE "\(.*\)/.*" \\1 sgbd_group_name ${sgbd_fpath})
        string(COMPARE EQUAL ${sgbd_fpath} ${sgbd_group_name} sgbd_nogroup)
        string(REPLACE "/" "\\" sgbd_group_name ${sgbd_group_name})
        if(sgbd_nogroup)
            set(sgbd_group_name "\\")
        endif(sgbd_nogroup)
		#message("The Group: " ${sgbd_group_name})
        source_group(${sgbd_group_name} FILES ${sgbd_file})
    endforeach(sgbd_file)
endmacro(source_group_by_dir)

#copy res
#define
add_definitions(-DB3_USE_CLEW)

file(GLOB_RECURSE project_headers ${_src_root_path}/*.h ${_src_root_path}/*.hpp)
file(GLOB_RECURSE project_cpps ${_src_root_path}/*.cpp ${_src_root_path}/*.c)

# Add external directory source files
file(GLOB_RECURSE external_headers ${CMAKE_CURRENT_SOURCE_DIR}/external/*.h ${CMAKE_CURRENT_SOURCE_DIR}/external/*.hpp)
file(GLOB_RECURSE external_cpps ${CMAKE_CURRENT_SOURCE_DIR}/external/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/external/*.c)

set(all_files ${project_headers} ${project_cpps} ${external_headers} ${external_cpps})
source_group_by_dir(all_files)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories(${CMAKE_BUILD_FILES_DIRECTORY}) # fixes syntax analysis in CLion
set(EngineSrcDir "./CubeEngine/EngineSrc/")
INCLUDE_DIRECTORIES(${CMAKE_BUILD_FILES_DIRECTORY} ${EngineSrcDir})
set(TARGET ${CMAKE_PROJECT_NAME})
add_executable(${TARGET} ${all_files})
target_link_libraries(${TARGET} ${QMAKE_LFLAGS})
target_link_libraries(${TARGET} ${LIBS})
