#include "Model.h"

#include "ModelLoader.h"
#include "../../Rendering/Renderer.h"
#include "../../Scene/SceneMgr.h"
#include "Utility/misc/Tmisc.h"
namespace tzw {

Model::Model():m_currPose(-1)
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
    reCache();
    reCacheAABB();
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
	if(getIsVisible())
	{
		auto type = passType;
		if(m_currPose == -1)
		{
		    for(auto mesh : m_meshList)
		    {
		        auto tech = m_effectList[0];
		        RenderCommand command(mesh,tech,type);
    			setUpCommand(command);
		        setUpTransFormation(command.m_transInfo);
		        Renderer::shared()->addRenderCommand(command);
		    }
		}else
		{
		    for(auto mesh : m_extraMeshList[m_currPose])
		    {
		        auto tech = m_effectList[0];
		        RenderCommand command(mesh,tech,type);
    			setUpCommand(command);
		        setUpTransFormation(command.m_transInfo);
		        Renderer::shared()->addRenderCommand(command);
		    }
		}

    }
}

Mesh * Model::getMesh(int id)
{
	return m_meshList[id];
}

Mesh* Model::getMesh()
{
	return m_meshList[0];
}

Material* Model::getMat(int index)
{
	return m_effectList[index];
}

void Model::setMaterial(Material* mat)
{
	m_effectList[0] = mat;
}

void Model::setColor(vec4 color)
{
	Drawable::setColor(color);
	m_effectList[0]->setVar("TU_color", color);
}

std::vector<Mesh*> Model::getMeshList()
{
	return m_meshList;
}

void Model::setMeshList(std::vector<Mesh*> newMeshList)
{
	m_meshList = newMeshList;
}

int Model::addExtraMeshList(std::vector<Mesh*> newMeshList)
{
	m_extraMeshList.push_back(newMeshList);
	return m_extraMeshList.size() - 1;
}

int Model::getCurrPose() const
{
	return m_currPose;
}

void Model::setCurrPose(const int currPose)
{
	m_currPose = currPose;
}
} // namespace tzw
