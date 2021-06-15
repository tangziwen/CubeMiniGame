#include "RenderQueues.h"
#include "3D/Vegetation/FoliageSystem.h"
#include "Rendering/InstancingMgr.h"
#include "../3D/ShadowMap/ShadowMap.h"
namespace tzw
{
	RenderQueue::RenderQueue()
	{
		m_instancesBatcher = new InstancingMgr();
	}

	void RenderQueue::addRenderCommand(RenderCommand& command, int level)
	{
		m_fullList.emplace_back(command);
	}

	void RenderQueue::addInstancedData(InstanceRendereData& data)
	{
		m_instancesBatcher->pushInstanceRenderData(RenderFlag::RenderStage::COMMON, data, 0);
	}

	std::vector<RenderCommand>& RenderQueue::getList()
	{
		return m_fullList;
	}

	void RenderQueue::clearCommands()
	{
		m_fullList.clear();
		m_instancesBatcher->prepare(RenderFlag::RenderStage::All, -1);
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
	void RenderQueue::generateInstancedDrawCall()
	{
		m_instancesBatcher->generateDrawCall(RenderFlag::RenderStage::COMMON, this, 0, 0);
	}
}
