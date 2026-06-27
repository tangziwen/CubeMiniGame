#include "SceneCuller.h"
#include "SceneMgr.h"
#include "OctreeScene.h"
#include "3D/Vegetation/FoliageSystem.h"
#include "Rendering/InstancingMgr.h"
#include "../3D/ShadowMap/ShadowMap.h"
#include "Engine/DebugSystem.h"
namespace tzw
{
	namespace
	{
		constexpr uint32_t SceneRenderStageMask =
			static_cast<uint32_t>(RenderFlag::RenderStage::COMMON)
			| static_cast<uint32_t>(RenderFlag::RenderStage::TRANSPARENT)
			| static_cast<uint32_t>(RenderFlag::RenderStage::AFTER_DEPTH_CLEAR)
			| static_cast<uint32_t>(RenderFlag::RenderStage::DEBUG_LAYER);
	}

	SceneCuller::SceneCuller()
	{
		m_renderQueues = new RenderQueue();
		for(int i = 0; i < 3; i++)
		{
			m_CSMQueue[i] = new RenderQueue();
		}
	}
	void SceneCuller::collectPrimitives()
	{
		clearCommands();
		auto currScene = SceneMgr::shared()->getCurrScene();
		std::vector<Node *> directDrawList = currScene->getDirectDrawList();
		for(auto node : directDrawList)
		{
			node->submitDrawCmd(static_cast<RenderFlag::RenderStage>(SceneRenderStageMask), m_renderQueues, 0);
			if(node->onSubmitDrawCommand)
			{
				node->onSubmitDrawCommand(static_cast<RenderFlag::RenderStage>(SceneRenderStageMask));
			}
	
		}
		directDrawList.clear();
		auto cam = SceneMgr::shared()->getCurrScene()->defaultCamera();
		OctreeScene * octreeScene =  SceneMgr::shared()->getCurrScene()->getOctreeScene();
		SceneMgr::shared()->getCurrScene()->getOctreeScene()->cullingByCamera(cam, SceneRenderStageMask);
		//vegetation
		FoliageSystem::shared()->clearTreeGroup();
		auto &visibleList = octreeScene->getVisibleList();
		for(auto obj : visibleList)
		{
			obj->submitDrawCmd(static_cast<RenderFlag::RenderStage>(SceneRenderStageMask), m_renderQueues, 0);
			if(obj->onSubmitDrawCommand)
			{
				obj->onSubmitDrawCommand(static_cast<RenderFlag::RenderStage>(SceneRenderStageMask));
			}
		}
		FoliageSystem::shared()->pushCommand(RenderFlag::RenderStage::COMMON, m_renderQueues, 0);

		
		std::vector<Drawable3D *> nodeList;
		octreeScene->cullingByCameraExtraFlag(cam, static_cast<uint32_t>(DrawableFlag::Instancing), SceneRenderStageMask, nodeList);
		InstancingMgr::shared()->prepare(RenderFlag::RenderStage::COMMON, -1);
		std::vector<InstanceRendereData> instanceDataList;
		for(auto node:nodeList)
		{
			if(node->getIsVisible())
			{
				const size_t startIndex = instanceDataList.size();
				node->getInstancedData(instanceDataList);   
				for(size_t i = startIndex; i < instanceDataList.size(); ++i)
				{
					instanceDataList[i].renderStageMask = node->getRenderStageForRequest(
						instanceDataList[i].material, SceneRenderStageMask);
				}
			}
		}
		for(auto& instanceData : instanceDataList)
		{
			if(instanceData.renderStageMask == static_cast<uint32_t>(RenderFlag::RenderStage::Unset))
			{
				continue;
			}
			m_renderQueues->addInstancedData(instanceData);
		}
		m_renderQueues->generateInstancedDrawCall();

		DebugSystem::shared()->doRender(m_renderQueues, 0.0);

		collectShadowCmd();
	}

	void SceneCuller::clearCommands()
	{
		m_renderQueues->clearCommands();
		for(int i = 0; i < 3; i++)
		{
			m_CSMQueue[i]->clearCommands();
		}
	}

	RenderQueue* SceneCuller::getRenderQueues()
	{
		return m_renderQueues;
	}

	RenderQueue* SceneCuller::getCSMQueues(int layer)
	{
		return m_CSMQueue[layer];
	}

	void SceneCuller::collectShadowCmd()
	{
		ShadowMap::shared()->calculateProjectionMatrix();
		for(int i = 0; i < 3; i ++)
		{
		
			auto aabb = ShadowMap::shared()->getPotentialRange(i);
			std::vector<Drawable3D *> shadowNeedDrawList;
		    g_GetCurrScene()->getRange(&shadowNeedDrawList, static_cast<uint32_t>(DrawableFlag::Drawable) | static_cast<uint32_t>(DrawableFlag::Instancing), 
				static_cast<uint32_t>(RenderFlag::RenderStage::SHADOW), aabb);

			
			
		    std::vector<InstanceRendereData> istanceCommandList;
		    for(auto obj:shadowNeedDrawList)
		    {
			    if(!obj->getIsVisible()) continue;
			    if(obj->getDrawableFlag() &static_cast<uint32_t>(DrawableFlag::Drawable))
			    {
				    obj->submitDrawCmd(RenderFlag::RenderStage::SHADOW, m_CSMQueue[i], i);
			    }
			    else//instancing
			    {
				    const size_t startIndex = istanceCommandList.size();
				    obj->getInstancedData(istanceCommandList);   
				    for(size_t dataIndex = startIndex; dataIndex < istanceCommandList.size(); ++dataIndex)
				    {
					    istanceCommandList[dataIndex].renderStageMask = obj->getRenderStageForRequest(
						    istanceCommandList[dataIndex].material,
						    static_cast<uint32_t>(RenderFlag::RenderStage::SHADOW));
				    }
			    }
		    }
			FoliageSystem::shared()->submitShadowDraw(m_CSMQueue[i], i);
			
			
		    for(auto& instanceData : istanceCommandList)
	        {
				if(instanceData.renderStageMask == static_cast<uint32_t>(RenderFlag::RenderStage::Unset))
				{
					continue;
				}
				m_CSMQueue[i]->addInstancedData(instanceData);
	        }
            m_CSMQueue[i]->generateInstancedDrawCall();
		}
	}

}
