#include "SceneCuller.h"
#include "SceneMgr.h"
#include "OctreeScene.h"
#include "3D/Vegetation/FoliageSystem.h"
#include "Rendering/InstancingMgr.h"
#include "../3D/ShadowMap/ShadowMap.h"
namespace tzw
{
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
			node->submitDrawCmd(RenderFlag::RenderStage::COMMON, m_renderQueues, 0);
			if(node->onSubmitDrawCommand)
			{
				node->onSubmitDrawCommand(RenderFlag::RenderStage::COMMON);
			}
	
		}
		directDrawList.clear();
		auto cam = SceneMgr::shared()->getCurrScene()->defaultCamera();
		OctreeScene * octreeScene =  SceneMgr::shared()->getCurrScene()->getOctreeScene();
		SceneMgr::shared()->getCurrScene()->getOctreeScene()->cullingByCamera(cam, static_cast<uint32_t>(RenderFlag::RenderStage::COMMON));
		//vegetation
		FoliageSystem::shared()->clearTreeGroup();
		auto &visibleList = octreeScene->getVisibleList();
		for(auto obj : visibleList)
		{
			obj->submitDrawCmd(RenderFlag::RenderStage::COMMON, m_renderQueues, 0);
			if(obj->onSubmitDrawCommand)
			{
				obj->onSubmitDrawCommand(RenderFlag::RenderStage::COMMON);
			}
		}
		FoliageSystem::shared()->pushCommand(RenderFlag::RenderStage::COMMON, m_renderQueues, 0);

		
		std::vector<Drawable3D *> nodeList;
		octreeScene->cullingByCameraExtraFlag(cam, static_cast<uint32_t>(DrawableFlag::Instancing),static_cast<uint32_t>(RenderFlag::RenderStage::COMMON), nodeList);
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
			m_renderQueues->addInstancedData(instanceData);
		}
		m_renderQueues->generateInstancedDrawCall();

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
				    obj->submitDrawCmd(RenderFlag::RenderStage::COMMON, m_CSMQueue[i], i);
			    }
			    else//instancing
			    {
				    obj->getInstancedData(istanceCommandList);   
			    }
		    }
			FoliageSystem::shared()->submitShadowDraw(m_CSMQueue[i], i);
			
			
		    for(auto& instanceData : istanceCommandList)
	        {
				m_CSMQueue[i]->addInstancedData(instanceData);
	        }
            m_CSMQueue[i]->generateInstancedDrawCall();
		}
	}

}