#include "CubePrimitive.h"
#include "../../Rendering/RenderCommand.h"
#include "../../Rendering/Renderer.h"
#include "../../Scene/SceneMgr.h"
namespace tzw {

CubePrimitive::CubePrimitive(float width, float depth, float height)
{
    m_width = width;
    m_depth = depth;
    m_height = height;
    m_tech = new Technique("./Res/EngineCoreRes/Shaders/Color_v.glsl",
                           "./Res/EngineCoreRes/Shaders/Color_f.glsl");
    initMesh();
    setCamera(SceneMgr::shared()->currentScene()->defaultCamera());
    setIsAccpectOCTtree(false);
}

void CubePrimitive::draw()
{
    m_tech->applyFromDrawable(this);
    //m_tech->setVar("TU_roughness",0.2f);
    RenderCommand command(m_mesh,m_tech,RenderCommand::RenderType::Common);
    //command.setPrimitiveType( RenderCommand::PrimitiveType::TRIANGLE_STRIP);
    Renderer::shared()->addRenderCommand(command);

}

void CubePrimitive::initMesh()
{
    auto halfWidth = m_width/2.0f;
    auto halfDepth = m_depth/2.0f;
    auto halfHeight = m_height/2.0f;
    m_mesh = new Mesh();
    VertexData vertices[] = {
        // Vertex data for face 0
        {vec3(-1.0f *halfWidth, -1.0f * halfHeight,  1.0f * halfDepth), vec2(0.0f, 0.0f)},  // v0
        {vec3( 1.0f *halfWidth, -1.0f * halfHeight,  1.0f * halfDepth), vec2(0.33f, 0.0f)}, // v1
        {vec3(-1.0f *halfWidth,  1.0f * halfHeight,  1.0f * halfDepth), vec2(0.0f, 0.5f)},  // v2
        {vec3( 1.0f *halfWidth,  1.0f * halfHeight,  1.0f * halfDepth), vec2(0.33f, 0.5f)}, // v3

        // Vertex data for face 1
        {vec3( 1.0f *halfWidth, -1.0f * halfHeight,  1.0f * halfDepth), vec2( 0.0f, 0.5f)}, // v4
        {vec3( 1.0f *halfWidth, -1.0f * halfHeight, -1.0f * halfDepth), vec2(0.33f, 0.5f)}, // v5
        {vec3( 1.0f *halfWidth,  1.0f * halfHeight,  1.0f * halfDepth), vec2(0.0f, 1.0f)},  // v6
        {vec3( 1.0f *halfWidth,  1.0f * halfHeight, -1.0f * halfDepth), vec2(0.33f, 1.0f)}, // v7

        // Vertex data for face 2
        {vec3( 1.0f *halfWidth, -1.0f * halfHeight, -1.0f * halfDepth), vec2(0.66f, 0.5f)}, // v8
        {vec3(-1.0f *halfWidth, -1.0f * halfHeight, -1.0f * halfDepth), vec2(1.0f, 0.5f)},  // v9
        {vec3( 1.0f *halfWidth,  1.0f * halfHeight, -1.0f * halfDepth), vec2(0.66f, 1.0f)}, // v10
        {vec3(-1.0f *halfWidth,  1.0f * halfHeight, -1.0f * halfDepth), vec2(1.0f, 1.0f)},  // v11

        // Vertex data for face 3
        {vec3(-1.0f *halfWidth, -1.0f * halfHeight, -1.0f * halfDepth), vec2(0.66f, 0.0f)}, // v12
        {vec3(-1.0f *halfWidth, -1.0f * halfHeight,  1.0f * halfDepth), vec2(1.0f, 0.0f)},  // v13
        {vec3(-1.0f *halfWidth,  1.0f * halfHeight, -1.0f * halfDepth), vec2(0.66f, 0.5f)}, // v14
        {vec3(-1.0f *halfWidth,  1.0f * halfHeight,  1.0f * halfDepth), vec2(1.0f, 0.5f)},  // v15

        // Vertex data for face 4
        {vec3(-1.0f *halfWidth, -1.0f * halfHeight, -1.0f * halfDepth), vec2(0.33f, 0.0f)}, // v16
        {vec3( 1.0f *halfWidth, -1.0f * halfHeight, -1.0f * halfDepth), vec2(0.66f, 0.0f)}, // v17
        {vec3(-1.0f *halfWidth, -1.0f * halfHeight,  1.0f * halfDepth), vec2(0.33f, 0.5f)}, // v18
        {vec3( 1.0f *halfWidth, -1.0f * halfHeight,  1.0f * halfDepth), vec2(0.66f, 0.5f)}, // v19

        // Vertex data for face 5
        {vec3(-1.0f *halfWidth,  1.0f * halfHeight,  1.0f * halfDepth), vec2(0.33f, 0.5f)}, // v20
        {vec3( 1.0f *halfWidth,  1.0f * halfHeight,  1.0f * halfDepth), vec2(0.66f, 0.5f)}, // v21
        {vec3(-1.0f *halfWidth,  1.0f * halfHeight, -1.0f * halfDepth), vec2(0.33f, 1.0f)}, // v22
        {vec3( 1.0f *halfWidth,  1.0f * halfHeight, -1.0f * halfDepth), vec2(0.66f, 1.0f)}  // v23
    };

    // Indices for drawing cube faces using triangle strips.
    // Triangle strips can be connected by duplicating indices
    // between the strips. If connecting strips have opposite
    // vertex order then last index of the first strip and first
    // index of the second strip needs to be duplicated. If
    // connecting strips have same vertex order then only last
    // index of the first strip needs to be duplicated.
    GLushort indices[] = {
         0,  1,  2,  1,  3,  2,     // Face 0 - triangle strip ( v0,  v1,  v2,  v3)
         4,  5,  6,  5,  7,  6, // Face 1 - triangle strip ( v4,  v5,  v6,  v7)
         8,  9,  10, 9, 11, 10, // Face 2 - triangle strip ( v8,  v9, v10, v11)
        12, 13, 14, 13, 15, 14, // Face 3 - triangle strip (v12, v13, v14, v15)
        16, 17, 18, 18, 19, 17, // Face 4 - triangle strip (v16, v17, v18, v19)
        20, 21, 22, 21, 23, 22,      // Face 5 - triangle strip (v20, v21, v22, v23)
    };

    m_mesh->addVertices(vertices,sizeof(vertices)/sizeof(VertexData));
    m_mesh->addIndices(indices,sizeof(indices)/sizeof(GLushort));
    m_mesh->caclNormals();
    m_mesh->finish();
}

} // namespace tzw
