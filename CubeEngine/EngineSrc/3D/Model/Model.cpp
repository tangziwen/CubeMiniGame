#include "Model.h"

#include "ModelLoader.h"
#include "../../External/TUtility/TUtility.h"
#include "../../Rendering/Renderer.h"
#include "../../Scene/SceneMgr.h"

namespace tzw {

Model::Model()
{

}

void Model::initWithFile(std::string modelFilePath)
{
    ModelLoader::shared()->loadModel(this,modelFilePath);
    setCamera(g_GetCurrScene()->defaultCamera());
    setIsAccpectOCTtree(false);
}

Model *Model::create(std::string modelFilePath)
{
    auto theModel = new Model();
    theModel->initWithFile(modelFilePath);
    return theModel;
}

void Model::submitDrawCmd()
{
    for(auto mesh : m_meshList)
    {
        auto tech = m_effectList[mesh->getMatIndex() - 1 ];
        RenderCommand command(mesh,tech,RenderCommand::RenderType::Common);
        setUpTransFormation(command.m_transInfo);
        Renderer::shared()->addRenderCommand(command);
    }
}
} // namespace tzw
