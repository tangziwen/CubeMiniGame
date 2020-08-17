#include "SceneCuller.h"
#include "SceneMgr.h"
#include "OctreeScene.h"
#include "3D/Vegetation/Tree.h"
#include "Rendering/InstancingMgr.h"
namespace tzw
{
	void SceneCuller::collectPrimitive()
	{
		auto currScene = SceneMgr::shared()->getCurrScene();
		std::vector<Node *> directDrawList = currScene->getDirectDrawList();
		for(auto node : directDrawList)
		{
			node->submitDrawCmd(RenderFlag::RenderStage::COMMON);
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
			obj->submitDrawCmd(RenderFlag::RenderStage::COMMON);
			if(obj->onSubmitDrawCommand)
			{
				obj->onSubmitDrawCommand(RenderFlag::RenderStage::COMMON);
			}
		}
		Tree::shared()->pushCommand();
		std::vector<Drawable3D *> nodeList;
		octreeScene->cullingByCameraExtraFlag(cam, static_cast<uint32_t>(DrawableFlag::Instancing), nodeList);
		InstancingMgr::shared()->prepare(RenderFlag::RenderStage::COMMON);
		std::vector<InstanceRendereData> istanceCommandList;
		for(auto node:nodeList)
		{
			if(node->getIsVisible())
			{
				node->getCommandForInstanced(istanceCommandList);   
			}
		}
		for(auto& instanceData : istanceCommandList)
		{
			InstancingMgr::shared()->pushInstanceRenderData(RenderFlag::RenderStage::COMMON, instanceData);
		}
		InstancingMgr::shared()->generateDrawCall(RenderFlag::RenderStage::COMMON);
	}

	void SceneCuller::addRenderCommand(RenderCommand& command)
	{
		switch (command.getRenderState())
		{
			case RenderFlag::RenderStage::SHADOW: m_shadowList.push_back(command);break;
			case RenderFlag::RenderStage::COMMON: m_commonList.push_back(command);break;
			case RenderFlag::RenderStage::TRANSPARENT: m_transparentList.push_back(command);break;
			case RenderFlag::RenderStage::AFTER_DEPTH_CLEAR: m_afterDepthList.push_back(command);break;
				case RenderFlag::RenderStage::GUI: m_guiCommandList.push_back(command); break;
		default: ;
		}
	}

	std::vector<RenderCommand>& SceneCuller::getShadowList()
	{
		return m_shadowList;
	}

	std::vector<RenderCommand>& SceneCuller::getCommonList()
	{
		return m_commonList;
	}

	std::vector<RenderCommand>& SceneCuller::getGUICommandList()
	{
		return m_guiCommandList;
	}

	std::vector<RenderCommand>& SceneCuller::getTransparentList()
	{
		return m_transparentList;
	}

	std::vector<RenderCommand>& SceneCuller::getAfterDepthList()
	{
		return m_afterDepthList;
	}

	void SceneCuller::clearCommands()
	{
		m_shadowList.clear();
		m_commonList.clear();
		m_guiCommandList.clear();
		m_transparentList.clear();
		m_afterDepthList.clear();
	}

}