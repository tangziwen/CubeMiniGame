#include "VoxelChunk.h"
#include "MCTable.h"
#include "../../Rendering/Renderer.h"
#include "../../Scene/SceneMgr.h"
#include "MarchingCubes.h"
namespace tzw {

VoxelChunk::VoxelChunk()
{
    m_mesh = new Mesh();
    m_technique = new Technique("./Res/EngineCoreRes/Shaders/GeometryPassColor_v.glsl","./Res/EngineCoreRes/Shaders/GeometryPassColor_f.glsl", nullptr, nullptr);
    m_technique->setVar("TU_roughness",0.1f);
    setIsAccpectOCTtree(false);
    setCamera(SceneMgr::shared()->currentScene()->defaultCamera());
    printf("the indices size%d \n", m_mesh->m_indices.size());
}

void VoxelChunk::draw()
{
    m_technique->applyFromDrawable(this);
    m_technique->use();

    //m_technique->applyFromMat(m_mesh->getMat());
    RenderCommand command(m_mesh,m_technique,RenderCommand::RenderType::Common);
    Renderer::shared()->addRenderCommand(command);
}

void VoxelChunk::init(int ncellsX, int ncellsY, int ncellsZ, vec4 *points, float minValue)
{
    MarchingCubes::shared()->generate(m_mesh, ncellsX, ncellsY, ncellsZ, points, minValue);
}

} // namespace tzw
