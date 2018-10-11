#include "Model.h"

#include "ModelLoader.h"
#include "../../Rendering/Renderer.h"
#include "../../Scene/SceneMgr.h"
#include "Utility/misc/Tmisc.h"
namespace tzw {

Model::Model()
{

}

void Model::initWithFile(std::string modelFilePath)
{
    ModelLoader::shared()->loadModel(this,modelFilePath);
    setCamera(g_GetCurrScene()->defaultCamera());
    for(auto mesh : m_meshList)
    {
		mesh->calculateAABB();
		m_localAABB.merge(mesh->getAabb());
	}
    setIsAccpectOcTtree(true);
}

Model *Model::create(std::string modelFilePath)
{
    auto theModel = new Model();
    theModel->initWithFile(modelFilePath);
    return theModel;
}

void Model::submitDrawCmd(RenderCommand::RenderType passType)
{
	auto type = passType;
    for(auto mesh : m_meshList)
    {
        auto tech = m_effectList[0];
        RenderCommand command(mesh,tech,type);
        setUpTransFormation(command.m_transInfo);
        Renderer::shared()->addRenderCommand(command);
    }
}

tzw::Mesh * Model::getMesh(int id)
{
	return m_meshList[id];
}

} // namespace tzw
