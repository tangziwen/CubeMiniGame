#include "SceneCuller.h"
#include "SceneMgr.h"
#include "OctreeScene.h"
#include "3D/Vegetation/Tree.h"
#include "Rendering/InstancingMgr.h"
#include "../3D/ShadowMap/ShadowMap.h"
namespace tzw
{
	SceneCuller::SceneCuller()
	{
		m_renderQueues = new RenderQueues();
	}
	void SceneCuller::collectPrimitives()
	{
		auto currScene = SceneMgr::shared()->getCurrScene();
		std::vector<Node *> directDrawList = currScene->getDirectDrawList();
		for(auto node : directDrawList)
		{
			node->submitDrawCmd(RenderFlag::RenderStageType::COMMON, m_renderQueues, 0);
			if(node->onSubmitDrawCommand)
			{
				node->onSubmitDrawCommand(RenderFlag::RenderStage::COMMON);
			}
	
		}
		directDrawList.clear();
		auto cam = SceneMgr::shared()->getCurrScene()->defaultCamera();
		OctreeScene * octreeScene =  SceneMgr::shared()->getCurrScene()->getOctreeScene();
		SceneMgr::shared()->getCurrScene()->getOctreeScene()->cullingByCamera(cam);
		//vegetation
		Tree::shared()->clearTreeGroup();
		auto &visibleList = octreeScene->getVisibleList();
		for(auto obj : visibleList)
		{
			obj->submitDrawCmd(RenderFlag::RenderStageType::COMMON, m_renderQueues, 0);
			if(obj->onSubmitDrawCommand)
			{
				obj->onSubmitDrawCommand(RenderFlag::RenderStage::COMMON);
			}
		}
		Tree::shared()->pushCommand(RenderFlag::RenderStageType::COMMON, m_renderQueues, 0);
		std::vector<Drawable3D *> nodeList;
		octreeScene->cullingByCameraExtraFlag(cam, static_cast<uint32_t>(DrawableFlag::Instancing), nodeList);
		InstancingMgr::shared()->prepare(RenderFlag::RenderStage::COMMON, -1);
		std::vector<InstanceRendereData> instanceDataList;
		for(auto node:nodeList)
		{
			if(node->getIsVisible())
			{
				node->getInstancedData(instanceDataList);   
			}
		}
		for(auto& instanceData : instanceDataList)
		{
			InstancingMgr::shared()->pushInstanceRenderData(RenderFlag::RenderStage::COMMON, instanceData, 0);
		}
		InstancingMgr::shared()->generateDrawCall(RenderFlag::RenderStageType::COMMON, m_renderQueues,0,0);

		collectShadowCmd();
	}

	void SceneCuller::clearCommands()
	{
		m_renderQueues->clearCommands();
	}

	RenderQueues* SceneCuller::getRenderQueues()
	{
		return m_renderQueues;
	}

	void SceneCuller::collectShadowCmd()
	{
		ShadowMap::shared()->calculateProjectionMatrix();
		InstancingMgr::shared()->prepare(RenderFlag::RenderStage::SHADOW, -1);
		for(int i = 0; i < 3; i ++)
		{
		
			auto aabb = ShadowMap::shared()->getPotentialRange(i);
			std::vector<Drawable3D *> shadowNeedDrawList;
		    g_GetCurrScene()->getRange(&shadowNeedDrawList, static_cast<uint32_t>(DrawableFlag::Drawable) | static_cast<uint32_t>(DrawableFlag::Instancing), aabb);
            
		    std::vector<InstanceRendereData> istanceCommandList;
		    for(auto obj:shadowNeedDrawList)
		    {
			    if(!obj->getIsVisible()) continue;
			    if(obj->getDrawableFlag() &static_cast<uint32_t>(DrawableFlag::Drawable))
			    {
				    obj->submitDrawCmd(RenderFlag::RenderStageType::COMMON, m_renderQueues, i);
			    }
			    else//instancing
			    {
				    obj->getInstancedData(istanceCommandList);   
			    }
		    }
		    for(auto& instanceData : istanceCommandList)
	        {
		        InstancingMgr::shared()->pushInstanceRenderData(RenderFlag::RenderStage::SHADOW, instanceData, i);
	        }
            Tree::shared()->submitShadowDraw(m_renderQueues, i);
            InstancingMgr::shared()->generateDrawCall(RenderFlag::RenderStageType::SHADOW, m_renderQueues, i, i);
		}
	}

}