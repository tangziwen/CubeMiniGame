#include "SceneCuller.h"

#include "Scene.h"
#include "SceneMgr.h"
#include "OctreeScene.h"
#include "3D/Vegetation/FoliageSystem.h"
#include "Interface/Drawable3D.h"
#include "Rendering/InstancingMgr.h"
#include "Rendering/RenderView.h"
#include "../3D/ShadowMap/ShadowMap.h"
#include "Engine/DebugSystem.h"

namespace tzw
{
	namespace
	{
		void collectSceneDirectDraw(RenderView* view);
		void collectSceneView(RenderView* view);
		void collectShadowView(RenderView* view);
	}

	SceneCuller::SceneCuller()
	{
	}

	void SceneCuller::collect(RenderView* view)
	{
		if(!view || !view->renderQueue())
		{
			return;
		}
		view->renderQueue()->clearCommands();

		switch(view->viewType())
		{
		case RenderViewType::Scene:
			collectSceneView(view);
			break;
		case RenderViewType::Shadow:
			collectShadowView(view);
			break;
		default:
			break;
		}
	}

	namespace
	{
	void collectSceneView(RenderView* view)
	{
		if(!view || !view->renderQueue())
		{
			return;
		}
		auto renderQueue = view->renderQueue();
		auto renderStageMask = view->submitStageMask();

		auto currScene = SceneMgr::shared()->getCurrScene();
		if(!currScene || !view->camera() || !currScene->getOctreeScene())
		{
			return;
		}

		collectSceneDirectDraw(view);

		auto cam = view->camera();
		OctreeScene * octreeScene = currScene->getOctreeScene();

		FoliageSystem::shared()->clearTreeGroup();
		std::vector<Drawable3D *> visibleList;
		octreeScene->cullingByCameraForRenderView(
			cam,
			view->viewType(),
			static_cast<uint32_t>(DrawableFlag::Drawable),
			renderStageMask,
			visibleList);
		for(auto obj : visibleList)
		{
			obj->submitDrawCmd(static_cast<RenderFlag::RenderStage>(renderStageMask), renderQueue, 0);
			if(obj->onSubmitDrawCommand)
			{
				obj->onSubmitDrawCommand(static_cast<RenderFlag::RenderStage>(renderStageMask));
			}
		}
		FoliageSystem::shared()->pushCommand(RenderFlag::RenderStage::COMMON, renderQueue, 0);

		std::vector<Drawable3D *> nodeList;
		octreeScene->cullingByCameraForRenderView(
			cam,
			view->viewType(),
			static_cast<uint32_t>(DrawableFlag::Instancing),
			renderStageMask,
			nodeList);
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
						instanceDataList[i].material, renderStageMask);
				}
			}
		}
		for(auto& instanceData : instanceDataList)
		{
			if(instanceData.renderStageMask == static_cast<uint32_t>(RenderFlag::RenderStage::Unset))
			{
				continue;
			}
			renderQueue->addInstancedData(instanceData, RenderFlag::RenderStage::COMMON, 0);
		}
		renderQueue->generateInstancedDrawCall(RenderFlag::RenderStage::COMMON, 0, 0);

		DebugSystem::shared()->doRender(renderQueue, 0.0);
	}

	void collectShadowView(RenderView* view)
	{
		if(!view || !view->renderQueue())
		{
			return;
		}
		auto renderQueue = view->renderQueue();
		auto renderStageMask = view->submitStageMask();
		int layer = view->viewIndex();

		auto currScene = g_GetCurrScene();
		if(!currScene || !currScene->getOctreeScene())
		{
			return;
		}

		auto aabb = ShadowMap::shared()->getPotentialRange(layer);
		std::vector<Drawable3D *> shadowNeedDrawList;
		currScene->getOctreeScene()->getRangeForRenderView(
			&shadowNeedDrawList,
			view->viewType(),
			static_cast<uint32_t>(DrawableFlag::Drawable) | static_cast<uint32_t>(DrawableFlag::Instancing),
			renderStageMask,
			aabb);

		std::vector<InstanceRendereData> instanceCommandList;
		for(auto obj:shadowNeedDrawList)
		{
			if(!obj->getIsVisible())
			{
				continue;
			}
			if(obj->getDrawableFlag() & static_cast<uint32_t>(DrawableFlag::Drawable))
			{
				obj->submitDrawCmd(static_cast<RenderFlag::RenderStage>(renderStageMask), renderQueue, layer);
			}
			else
			{
				const size_t startIndex = instanceCommandList.size();
				obj->getInstancedData(instanceCommandList);
				for(size_t dataIndex = startIndex; dataIndex < instanceCommandList.size(); ++dataIndex)
				{
					instanceCommandList[dataIndex].renderStageMask = obj->getRenderStageForRequest(
						instanceCommandList[dataIndex].material,
						renderStageMask);
				}
			}
		}
		if(FoliageSystem::shared()->isCastShadow())
		{
			FoliageSystem::shared()->submitShadowDraw(renderQueue, layer);
		}

		for(auto& instanceData : instanceCommandList)
		{
			if(instanceData.renderStageMask == static_cast<uint32_t>(RenderFlag::RenderStage::Unset))
			{
				continue;
			}
			renderQueue->addInstancedData(instanceData, RenderFlag::RenderStage::SHADOW, layer);
		}
		renderQueue->generateInstancedDrawCall(RenderFlag::RenderStage::SHADOW, layer, layer);
	}

	void collectSceneDirectDraw(RenderView* view)
	{
		auto currScene = SceneMgr::shared()->getCurrScene();
		auto renderQueue = view->renderQueue();
		auto renderStageMask = view->submitStageMask();
		std::vector<Node *> directDrawList = currScene->getDirectDrawList();
		for(auto node : directDrawList)
		{
			if(!node || node->getNodeType() != Node::NodeType::Drawable3D)
			{
				continue;
			}
			auto drawable = static_cast<Drawable3D*>(node);
			if(!drawable->acceptsRenderView(view->viewType()))
			{
				continue;
			}
			node->submitDrawCmd(static_cast<RenderFlag::RenderStage>(renderStageMask), renderQueue, view->viewIndex());
			if(node->onSubmitDrawCommand)
			{
				node->onSubmitDrawCommand(static_cast<RenderFlag::RenderStage>(renderStageMask));
			}
		}
	}
	}
}
