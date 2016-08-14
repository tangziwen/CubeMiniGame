#include "Block.h"


namespace tzw {

Block::Block()
{
    for(int i =0;i<3;i++)
    {
        m_faceVisibility[i] = true;
    }
}

Block *Block::create(std::string configFile)
{
    auto newBlock =  new Block();
    newBlock->m_info = BlockInfoMgr::shared()->createOrGetInfo(configFile);
    newBlock->init();
    return newBlock;
}

void Block::setVisible(int face, bool isVisible)
{
    m_faceVisibility[face] = isVisible;
}

void Block::draw()
{
    if(!m_faceVisibility[0]&&!m_faceVisibility[1] &&!m_faceVisibility[2])
        return;
    auto vp = camera()->getViewProjectionMatrix();
    auto m = getTransform();
    m_tech->setVar("TU_mvpMatrix", vp* m);
    m_tech->setVar("TU_color",m_uniformColor);
    RenderCommand command(m_info->mesh(),m_tech,RenderCommand::RenderType::Common);
    Renderer::shared()->addRenderCommand(command);
}

void Block::init()
{
    m_tech = new Technique("./Res/EngineCoreRes/Shaders/Simple_v.glsl","./Res/EngineCoreRes/Shaders/Simple_f.glsl");
    m_tech->setTex("TU_tex1",m_info->topTexture()->getTexture());
    setCamera(SceneMgr::shared()->currentScene()->defaultCamera());
    m_localAABB.merge(m_info->mesh()->getAabb());
    setIsAccpectOCTtree(false);
}

BlockInfo *Block::info() const
{
    return m_info;
}

} // namespace tzw

