get_filename_component(CUBEENGINE_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)

set(HEADER_ONLY_COMPANION_SOURCES
    "${CUBEENGINE_ROOT_DIR}/external/gli/core/dummy.cpp"
    "${CUBEENGINE_ROOT_DIR}/external/glm/detail/glm.cpp"
)

add_library(thirdparty_header_only STATIC ${HEADER_ONLY_COMPANION_SOURCES})
target_compile_features(thirdparty_header_only PUBLIC cxx_std_17)
target_include_directories(thirdparty_header_only
    PUBLIC
        "${CUBEENGINE_ROOT_DIR}/external"
        "${CUBEENGINE_ROOT_DIR}/external/gli"
        "${CUBEENGINE_ROOT_DIR}/external/glm"
        "${CUBEENGINE_ROOT_DIR}/external/rapidjson/include"
        "${CUBEENGINE_ROOT_DIR}/external/pybind11"
        "${CUBEENGINE_ROOT_DIR}/external/vulkan"
)

file(GLOB_RECURSE SOIL2_SOURCES CONFIGURE_DEPENDS
    "${CUBEENGINE_ROOT_DIR}/external/SOIL2/*.c"
    "${CUBEENGINE_ROOT_DIR}/external/SOIL2/*.cpp"
)
add_library(thirdparty_soil2 STATIC ${SOIL2_SOURCES})
target_include_directories(thirdparty_soil2
    PUBLIC
        "${CUBEENGINE_ROOT_DIR}/external/SOIL2"
)

file(GLOB_RECURSE NOISE_SOURCES CONFIGURE_DEPENDS
    "${CUBEENGINE_ROOT_DIR}/external/noise/*.cpp"
    "${CUBEENGINE_ROOT_DIR}/external/FastNoise/*.cpp"
)
add_library(thirdparty_noise STATIC ${NOISE_SOURCES})
target_compile_features(thirdparty_noise PUBLIC cxx_std_17)
target_include_directories(thirdparty_noise
    PUBLIC
        "${CUBEENGINE_ROOT_DIR}/external/noise"
        "${CUBEENGINE_ROOT_DIR}/external/FastNoise"
)

file(GLOB_RECURSE ZIP_SOURCES CONFIGURE_DEPENDS
    "${CUBEENGINE_ROOT_DIR}/external/zip/*.c"
    "${CUBEENGINE_ROOT_DIR}/external/zip/*.cpp"
)
add_library(thirdparty_zip STATIC ${ZIP_SOURCES})
target_include_directories(thirdparty_zip
    PUBLIC
        "${CUBEENGINE_ROOT_DIR}/external/zip"
)

if(CUBEENGINE_BUILD_BULLET_FROM_SOURCE)
    file(GLOB_RECURSE BULLET_SOURCES CONFIGURE_DEPENDS
        "${CUBEENGINE_ROOT_DIR}/external/Bullet/LinearMath/*.cpp"
        "${CUBEENGINE_ROOT_DIR}/external/Bullet/BulletCollision/*.cpp"
        "${CUBEENGINE_ROOT_DIR}/external/Bullet/BulletDynamics/*.cpp"
        "${CUBEENGINE_ROOT_DIR}/external/Bullet/BulletSoftBody/*.cpp"
        "${CUBEENGINE_ROOT_DIR}/external/Bullet/BulletInverseDynamics/*.cpp"
    )
    add_library(thirdparty_bullet STATIC ${BULLET_SOURCES})
    target_compile_features(thirdparty_bullet PUBLIC cxx_std_17)
    target_include_directories(thirdparty_bullet
        PUBLIC
            "${CUBEENGINE_ROOT_DIR}/external/Bullet"
    )
else()
    message(FATAL_ERROR "Prebuilt Bullet target is not configured yet; keep CUBEENGINE_BUILD_BULLET_FROM_SOURCE=ON.")
endif()

file(GLOB_RECURSE NODE_EDITOR_SOURCES CONFIGURE_DEPENDS
    "${CUBEENGINE_ROOT_DIR}/external/NodeEditor/Source/*.cpp"
    "${CUBEENGINE_ROOT_DIR}/external/NodeEditor/BlueprintUtilities/Source/*.cpp"
)
add_library(thirdparty_node_editor STATIC ${NODE_EDITOR_SOURCES})
target_compile_features(thirdparty_node_editor PUBLIC cxx_std_17)
target_include_directories(thirdparty_node_editor
    PUBLIC
        "${CUBEENGINE_ROOT_DIR}/external"
        "${CUBEENGINE_ROOT_DIR}/external/NodeEditor/Include"
        "${CUBEENGINE_ROOT_DIR}/external/NodeEditor/Source"
        "${CUBEENGINE_ROOT_DIR}/external/NodeEditor/BlueprintUtilities/Include"
)

set(CUBEENGINE_IMGUI_SOURCES
    "${CUBEENGINE_ROOT_DIR}/CubeEngine/EngineSrc/2D/imgui.cpp"
    "${CUBEENGINE_ROOT_DIR}/CubeEngine/EngineSrc/2D/imgui_demo.cpp"
    "${CUBEENGINE_ROOT_DIR}/CubeEngine/EngineSrc/2D/imgui_draw.cpp"
    "${CUBEENGINE_ROOT_DIR}/CubeEngine/EngineSrc/2D/imgui_widgets.cpp"
    "${CUBEENGINE_ROOT_DIR}/CubeEngine/EngineSrc/2D/imnodes.cpp"
)

set(CUBEENGINE_IMGUI_HEADERS
    "${CUBEENGINE_ROOT_DIR}/CubeEngine/EngineSrc/2D/imconfig.h"
    "${CUBEENGINE_ROOT_DIR}/CubeEngine/EngineSrc/2D/imgui.h"
    "${CUBEENGINE_ROOT_DIR}/CubeEngine/EngineSrc/2D/imgui_internal.h"
    "${CUBEENGINE_ROOT_DIR}/CubeEngine/EngineSrc/2D/imgui_markdown.h"
    "${CUBEENGINE_ROOT_DIR}/CubeEngine/EngineSrc/2D/imnodes.h"
    "${CUBEENGINE_ROOT_DIR}/CubeEngine/EngineSrc/2D/imstb_rectpack.h"
    "${CUBEENGINE_ROOT_DIR}/CubeEngine/EngineSrc/2D/imstb_textedit.h"
    "${CUBEENGINE_ROOT_DIR}/CubeEngine/EngineSrc/2D/imstb_truetype.h"
)

add_library(CubeEngineImGui STATIC
    ${CUBEENGINE_IMGUI_SOURCES}
    ${CUBEENGINE_IMGUI_HEADERS}
)
target_compile_features(CubeEngineImGui PUBLIC cxx_std_17)
target_include_directories(CubeEngineImGui
    PUBLIC
        "${CUBEENGINE_ROOT_DIR}/CubeEngine/EngineSrc"
        "${CUBEENGINE_ROOT_DIR}/CubeEngine/EngineSrc/2D"
)
source_group(TREE "${CUBEENGINE_ROOT_DIR}/CubeEngine/EngineSrc" FILES ${CUBEENGINE_IMGUI_SOURCES} ${CUBEENGINE_IMGUI_HEADERS})

target_link_libraries(thirdparty_node_editor
    PUBLIC
        CubeEngineImGui
)

foreach(CUBEENGINE_THIRDPARTY_TARGET
    thirdparty_header_only
    thirdparty_soil2
    thirdparty_noise
    thirdparty_zip
    thirdparty_bullet
    thirdparty_node_editor
    CubeEngineImGui
)
    if(MSVC)
        target_compile_options(${CUBEENGINE_THIRDPARTY_TARGET} PRIVATE /utf-8)
    endif()
endforeach()
