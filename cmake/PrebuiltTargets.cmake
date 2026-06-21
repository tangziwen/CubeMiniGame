get_filename_component(CUBEENGINE_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
set(CUBEENGINE_LIBVC_DIR "${CUBEENGINE_ROOT_DIR}/CubeEngine/libVC")

function(cubeengine_add_imported_library target_name library_name)
    set(library_path "${CUBEENGINE_LIBVC_DIR}/${library_name}.lib")
    if(NOT EXISTS "${library_path}")
        message(FATAL_ERROR "Missing prebuilt library for ${target_name}: ${library_path}")
    endif()

    add_library(${target_name} UNKNOWN IMPORTED GLOBAL)
    set_target_properties(${target_name} PROPERTIES
        IMPORTED_LOCATION "${library_path}"
    )
endfunction()

cubeengine_add_imported_library(thirdparty_freetype freetype)
set_target_properties(thirdparty_freetype PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${CUBEENGINE_ROOT_DIR}/external/FreeType"
)

cubeengine_add_imported_library(thirdparty_glfw3 glfw3)
set_target_properties(thirdparty_glfw3 PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${CUBEENGINE_ROOT_DIR}/external/GLFW3/include"
)

cubeengine_add_imported_library(thirdparty_glew glew32s)
set_target_properties(thirdparty_glew PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${CUBEENGINE_ROOT_DIR}/external/GLEW"
    INTERFACE_COMPILE_DEFINITIONS GLEW_STATIC
)

cubeengine_add_imported_library(thirdparty_vulkan vulkan-1)
set_target_properties(thirdparty_vulkan PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${CUBEENGINE_ROOT_DIR}/external/vulkan"
)

cubeengine_add_imported_library(thirdparty_spirv_cross_c spirv-cross-c)
cubeengine_add_imported_library(thirdparty_spirv_cross_core spirv-cross-core)
cubeengine_add_imported_library(thirdparty_spirv_cross_cpp spirv-cross-cpp)
cubeengine_add_imported_library(thirdparty_spirv_cross_glsl spirv-cross-glsl)
cubeengine_add_imported_library(thirdparty_spirv_cross_reflect spirv-cross-reflect)
cubeengine_add_imported_library(thirdparty_spirv_cross_util spirv-cross-util)
cubeengine_add_imported_library(thirdparty_spirv_cross_c_shared spirv-cross-c-shared)
cubeengine_add_imported_library(thirdparty_spirv_cross_hlsl spirv-cross-hlsl)
cubeengine_add_imported_library(thirdparty_spirv_cross_msl spirv-cross-msl)

add_library(thirdparty_spirv_cross INTERFACE)
target_include_directories(thirdparty_spirv_cross
    INTERFACE
        "${CUBEENGINE_ROOT_DIR}/external/spirv_cross"
)
target_link_libraries(thirdparty_spirv_cross
    INTERFACE
        thirdparty_spirv_cross_c
        thirdparty_spirv_cross_core
        thirdparty_spirv_cross_cpp
        thirdparty_spirv_cross_glsl
        thirdparty_spirv_cross_reflect
        thirdparty_spirv_cross_util
        thirdparty_spirv_cross_c_shared
        thirdparty_spirv_cross_hlsl
        thirdparty_spirv_cross_msl
)

cubeengine_add_imported_library(thirdparty_glslang glslang)
set_target_properties(thirdparty_glslang PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${CUBEENGINE_ROOT_DIR}/external/glslang"
)

cubeengine_add_imported_library(thirdparty_shaderc shaderc_shared)
set_target_properties(thirdparty_shaderc PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${CUBEENGINE_ROOT_DIR}/external/shaderc"
)

cubeengine_add_imported_library(thirdparty_python python314)
set_target_properties(thirdparty_python PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${CUBEENGINE_ROOT_DIR}/external/Python/include"
)

cubeengine_add_imported_library(thirdparty_fmod fmodex_vc)
set_target_properties(thirdparty_fmod PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${CUBEENGINE_ROOT_DIR}/external/FMod"
)

cubeengine_add_imported_library(thirdparty_soloud soloud_static)
set_target_properties(thirdparty_soloud PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${CUBEENGINE_ROOT_DIR}/external/SoLoud/include"
)

cubeengine_add_imported_library(thirdparty_box2d box2d)
set_target_properties(thirdparty_box2d PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${CUBEENGINE_ROOT_DIR}/external/box2d"
)

cubeengine_add_imported_library(thirdparty_zlib zlib)

cubeengine_add_imported_library(thirdparty_libpng libpng)

add_library(thirdparty_opengl INTERFACE)
target_link_libraries(thirdparty_opengl INTERFACE opengl32)

add_library(thirdparty_dbghelp INTERFACE)
target_link_libraries(thirdparty_dbghelp INTERFACE Dbghelp)
