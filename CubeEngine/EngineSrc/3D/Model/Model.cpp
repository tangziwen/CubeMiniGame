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
    setCamera(SceneMgr::shared()->currentScene()->defaultCamera());
    m_technique = new Technique("./Res/EngineCoreRes/Shaders/Simple_v.glsl","./Res/EngineCoreRes/Shaders/Simple_f.glsl");
    setIsAccpectOCTtree(false);
}

Model *Model::create(std::string modelFilePath)
{
    auto theModel = new Model();
    theModel->initWithFile(modelFilePath);
    return theModel;
}

void Model::draw()
{
    m_technique->applyFromDrawable(this);
    for(auto mesh : m_meshList)
    {
        m_technique->applyFromMat(mesh->getMat());
        RenderCommand command(mesh,m_technique,RenderCommand::RenderType::Common);
        Renderer::shared()->addRenderCommand(command);
    }
}
} // namespace tzw
