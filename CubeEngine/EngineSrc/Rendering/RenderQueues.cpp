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

	void RenderQueue::addInstancedData(InstanceRendereData& data, DrawPassTypeMask drawPassMask, int batchID)
	{
		m_instancesBatcher->pushInstanceRenderData(drawPassMask, data, batchID);
	}

	std::vector<RenderCommand>& RenderQueue::getList()
	{
		return m_fullList;
	}

	const std::vector<RenderCommand>& RenderQueue::getList() const
	{
		return m_fullList;
	}

	void RenderQueue::clearCommands()
	{
		m_fullList.clear();
		m_instancesBatcher->prepare(DrawPassType::All, -1);
	}

	void RenderQueue::dispatch(RenderQueue* otherQueue, DrawPassTypeMask drawPassMask)
	{
		otherQueue->clearCommands();
		for(auto& cmd :m_fullList)
		{
			if(cmd.getDrawPassMask() & drawPassMask)
			{
				otherQueue->addRenderCommand(cmd, 0);
			}
		}
	}

	void RenderQueue::generateInstancedDrawCall(DrawPassTypeMask drawPassMask, int batchID, int requirementArg)
	{
		m_instancesBatcher->generateDrawCall(drawPassMask, this, batchID, requirementArg);
	}
}
