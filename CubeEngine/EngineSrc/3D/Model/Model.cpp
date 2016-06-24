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
    m_technique = new Technique("./Res/EngineCoreRes/Shaders/Debug_v.glsl","./Res/EngineCoreRes/Shaders/Debug_f.glsl");
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
    auto vp = camera()->getViewProjectionMatrix();
    auto m = getTransform();
    m_technique->setVar("TU_mvpMatrix", vp* m);
    RenderCommand command(m_meshList[0],m_technique,RenderCommand::RenderType::Common);
    Renderer::shared()->addRenderCommand(command);
}

} // namespace tzw
