#include "Model.h"

#include "ModelLoader.h"
#include "../../Rendering/Renderer.h"
#include "../../Scene/SceneMgr.h"
#include "Utility/misc/Tmisc.h"
namespace tzw {

Model::Model():m_currPose(-1)
{

}

void Model::initWithFile(std::string modelFilePath, bool useCache)
{
    ModelLoader::shared()->loadModel(this,modelFilePath, useCache);
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

Model *Model::create(std::string modelFilePath, bool useCache)
{
    auto theModel = new Model();
    theModel->initWithFile(modelFilePath, useCache);
    return theModel;
}

void Model::submitDrawCmd(RenderFlag::RenderStage stageType, RenderQueue * queues, int requirementArg)
{
	if(getIsVisible())
	{
		if(m_currPose == -1)
		{
		    for(auto mesh : m_meshList)
		    {
		    	Material* mat = nullptr;
				if(mesh->getMatIndex() > m_effectList.size() - 1)
				{
					mat= m_effectList[0];
					//try to adjust it
				}
				else
				{
					mat= m_effectList[mesh->getMatIndex()];
				}
		        RenderCommand command(mesh,mat, this, stageType);
    			setUpCommand(command);
		        setUpTransFormation(command.m_transInfo);
		        queues->addRenderCommand(command, requirementArg);
		    }
		}else
		{
		    for(auto mesh : m_extraMeshList[m_currPose])
		    {
		        auto tech = m_effectList[mesh->getMatIndex()];
		        RenderCommand command(mesh,tech, this, stageType);
    			setUpCommand(command);
		        setUpTransFormation(command.m_transInfo);
		        queues->addRenderCommand(command, requirementArg);
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

Material* Model::getMaterial() const
{
	return m_effectList[0];
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

int Model::getMeshCount()
{
	return m_meshList.size();
}

int Model::getMatCount()
{
	return m_effectList.size();
}

void Model::setMeshToMatMap(Mesh* mesh, Material* mat)
{
	m_meshToMat[mesh] = mat;
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
