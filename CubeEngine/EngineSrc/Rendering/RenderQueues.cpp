#include "RenderQueues.h"
#include "3D/Vegetation/FoliageSystem.h"
#include "Rendering/InstancingMgr.h"
#include "../3D/ShadowMap/ShadowMap.h"
namespace tzw
{

	void RenderQueue::addRenderCommand(RenderCommand& command, int level)
	{
		m_fullList.emplace_back(command);
	}

	std::vector<RenderCommand>& RenderQueue::getList()
	{
		return m_fullList;
	}

	void RenderQueue::clearCommands()
	{
		m_fullList.clear();
	}

	void RenderQueue::dispatch(RenderQueue* otherQueue, uint32_t renderStage)
	{
		otherQueue->clearCommands();
		for(auto& cmd :m_fullList)
		{
			if((uint32_t)(cmd.getRenderState()) & renderStage)
			{
				otherQueue->addRenderCommand(cmd, 0);
			}
		}
	}
}
