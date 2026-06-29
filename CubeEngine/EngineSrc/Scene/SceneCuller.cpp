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
		auto drawPassMask = view->submitDrawPassMask();

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
			drawPassMask,
			visibleList);
		for(auto obj : visibleList)
		{
			obj->submitDrawCmd(drawPassMask, renderQueue, 0);
			if(obj->onSubmitDrawCommand)
			{
				obj->onSubmitDrawCommand(drawPassMask);
			}
		}
		FoliageSystem::shared()->pushCommand(DrawPassType::GBuffer, renderQueue, 0);

		std::vector<Drawable3D *> nodeList;
		octreeScene->cullingByCameraForRenderView(
			cam,
			view->viewType(),
			static_cast<uint32_t>(DrawableFlag::Instancing),
			drawPassMask,
			nodeList);
		InstancingMgr::shared()->prepare(DrawPassType::GBuffer, -1);
		std::vector<InstanceRendereData> instanceDataList;
		for(auto node:nodeList)
		{
			if(node->getIsVisible())
			{
				const size_t startIndex = instanceDataList.size();
				node->getInstancedData(instanceDataList);
				for(size_t i = startIndex; i < instanceDataList.size(); ++i)
				{
					instanceDataList[i].drawPassMask = node->getDrawPassForRequest(
						instanceDataList[i].material, drawPassMask);
				}
			}
		}
		for(auto& instanceData : instanceDataList)
		{
			if(instanceData.drawPassMask == DrawPassType::Unset)
			{
				continue;
			}
			renderQueue->addInstancedData(instanceData, DrawPassType::GBuffer, 0);
		}
		renderQueue->generateInstancedDrawCall(DrawPassType::GBuffer, 0, 0);

		DebugSystem::shared()->doRender(renderQueue, 0.0);
	}

	void collectShadowView(RenderView* view)
	{
		if(!view || !view->renderQueue())
		{
			return;
		}
		auto renderQueue = view->renderQueue();
		auto drawPassMask = view->submitDrawPassMask();
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
			drawPassMask,
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
				obj->submitDrawCmd(drawPassMask, renderQueue, layer);
			}
			else
			{
				const size_t startIndex = instanceCommandList.size();
				obj->getInstancedData(instanceCommandList);
				for(size_t dataIndex = startIndex; dataIndex < instanceCommandList.size(); ++dataIndex)
				{
					instanceCommandList[dataIndex].drawPassMask = obj->getDrawPassForRequest(
						instanceCommandList[dataIndex].material,
						drawPassMask);
				}
			}
		}
		if(FoliageSystem::shared()->isCastShadow())
		{
			FoliageSystem::shared()->submitShadowDraw(renderQueue, layer);
		}

		for(auto& instanceData : instanceCommandList)
		{
			if(instanceData.drawPassMask == DrawPassType::Unset)
			{
				continue;
			}
			renderQueue->addInstancedData(instanceData, DrawPassType::Shadow, layer);
		}
		renderQueue->generateInstancedDrawCall(DrawPassType::Shadow, layer, layer);
	}

	void collectSceneDirectDraw(RenderView* view)
	{
		auto currScene = SceneMgr::shared()->getCurrScene();
		auto renderQueue = view->renderQueue();
		auto drawPassMask = view->submitDrawPassMask();
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
			node->submitDrawCmd(drawPassMask, renderQueue, view->viewIndex());
			if(node->onSubmitDrawCommand)
			{
				node->onSubmitDrawCommand(drawPassMask);
			}
		}
	}
	}
}
