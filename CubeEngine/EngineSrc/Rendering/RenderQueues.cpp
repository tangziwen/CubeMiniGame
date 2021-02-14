#include "RenderQueues.h"
#include "3D/Vegetation/FoliageSystem.h"
#include "Rendering/InstancingMgr.h"
#include "../3D/ShadowMap/ShadowMap.h"
namespace tzw
{

	void RenderQueues::addRenderCommand(RenderCommand& command, int level)
	{
		switch (command.getRenderState())
		{
			case RenderFlag::RenderStage::SHADOW : m_shadowList[level].emplace_back(command);break;
			case RenderFlag::RenderStage::COMMON: m_commonList.emplace_back(command);break;
			case RenderFlag::RenderStage::TRANSPARENT: m_transparentList.emplace_back(command);break;
			case RenderFlag::RenderStage::AFTER_DEPTH_CLEAR: m_afterDepthList.emplace_back(command);break;
				case RenderFlag::RenderStage::GUI: m_guiCommandList.emplace_back(command); break;
		default: ;
		}
	}

	void RenderQueues::addShadowRenderCommand(RenderCommand& cmd, int level)
	{
		m_shadowList[level].emplace_back(cmd);
	}

	std::vector<RenderCommand>& RenderQueues::getShadowList(int index)
	{
		return m_shadowList[index];
	}

	std::vector<RenderCommand>& RenderQueues::getCommonList()
	{
		return m_commonList;
	}

	std::vector<RenderCommand>& RenderQueues::getGUICommandList()
	{
		return m_guiCommandList;
	}

	std::vector<RenderCommand>& RenderQueues::getTransparentList()
	{
		return m_transparentList;
	}

	std::vector<RenderCommand>& RenderQueues::getAfterDepthList()
	{
		return m_afterDepthList;
	}

	void RenderQueues::clearCommands()
	{
		for(int i = 0; i < 3; i++ )
		{
			m_shadowList[i].clear();
		}
		m_commonList.clear();
		m_guiCommandList.clear();
		m_transparentList.clear();
		m_afterDepthList.clear();
	}

}