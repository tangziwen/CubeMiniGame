#include "TerrainChunk.h"
#include "../../External/TUtility/TUtility.h"
#include "../../Rendering/Renderer.h"
#include "../../Scene/SceneMgr.h"
#include "noise/noise.h"
#include "noise/noiseutils.h"
#include "../../Texture/TextureMgr.h"
namespace tzw {

TerrainChunk::TerrainChunk()
{
    noise::module::Perlin myModule;
    auto m_plane = new noise::model::Plane(myModule);
    m_mesh = new Mesh();
    for (int z = 0; z < 128; z++)
    {
        for (int x = 0; x < 128; x++)
        {
            float height = m_plane->GetValue(x*0.05, z * 0.05);
            m_mesh->addVertex(VertexData(vec3(x * 1, height * 5.0f, z * 1),vec2(x / 128.f, z / 128.f)));
        }
    }
    for(int z = 0; z < 128 - 1; z++)
    {
        for(int x = 0; x < 128 - 1; x++)
        {
            auto theIndex = z * 128 + x;
            auto nextLineIndex = (z+1) * 128 + x;
            m_mesh->addIndex(theIndex);
            m_mesh->addIndex((z+1) * 128 + x);
            m_mesh->addIndex(theIndex + 1);

            m_mesh->addIndex(nextLineIndex);
            m_mesh->addIndex(nextLineIndex + 1);
            m_mesh->addIndex(theIndex + 1);
        }
    }
    m_mesh->caclNormals();
    // ambient occlusion
    for (int z = 0; z < 128; z++)
    {
        for (int x = 0; x < 128; x++)
        {
            auto theIndex = z * 128 + x;
            auto vertex = m_mesh->getVertex(theIndex);
            float occlusion = 0.0f;
            for(int i =0; i < 32; i++)
            {
                float s = float(i)/32.0;
                float a = sqrt(s*128.0);
                float b = sqrt(s);
                float x = sin(a)*b * 10;
                float z = cos(a)*b * 10;
                auto sample_dir = vec3(x, m_plane->GetValue(x*0.01, z * 0.01), z) - vertex.m_pos;
                float length = sample_dir.length();
                sample_dir.normalize();
                float lambert = TbaseMath::clampf(vec3::DotProduct(vertex.m_normal, sample_dir), 0.0, 1.0);
                float dist_factor = 0.23/sqrt(length);
                occlusion += dist_factor * lambert * 10;
            }
            float incident = 1.0 - occlusion/32.0;
            vertex.m_color = vec3(incident, incident, incident);
            m_mesh->setVertex(theIndex, vertex);
            float height = m_plane->GetValue(x*0.05, z * 0.05);
            m_mesh->addVertex(VertexData(vec3(x * 1, height * 5.0f, z * 1),vec2(x / 128.f, z / 128.f)));
        }
    }
    m_mesh->finish();

    m_technique = new Technique("./Res/EngineCoreRes/Shaders/Terrain_v.glsl","./Res/EngineCoreRes/Shaders/Terrain_f.glsl", nullptr, nullptr);
    m_technique->setVar("TU_roughness",0.1f);
    auto texture = TextureMgr::shared()->getOrCreateTexture("./Res/TestRes/rock_texture.png");
    auto rockNormalTexture = TextureMgr::shared()->getOrCreateTexture("./Res/TestRes/rock_normals.png");
    auto grassTexture = TextureMgr::shared()->getOrCreateTexture("./Res/TestRes/grass_texture.png");
    texture->setWarp(RenderFlag::WarpAddress::Repeat);
    m_technique->setTex("TU_tex1", texture, 0);
    m_technique->setTex("TU_rockNormal", rockNormalTexture, 1);
    m_technique->setTex("TU_grass", grassTexture, 2);
    setIsAccpectOCTtree(false);
    setCamera(SceneMgr::shared()->currentScene()->defaultCamera());
}

void TerrainChunk::draw()
{
    m_technique->applyFromDrawable(this);
    m_technique->use();

    //m_technique->applyFromMat(m_mesh->getMat());
    RenderCommand command(m_mesh,m_technique,RenderCommand::RenderType::Common);
    Renderer::shared()->addRenderCommand(command);
}

} // namespace tzw
