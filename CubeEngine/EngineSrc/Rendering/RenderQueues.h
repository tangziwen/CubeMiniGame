#pragma once
#include "Engine/EngineDef.h"
#include <functional>
#include <unordered_set>
#include <unordered_map>
#include "Rendering/RenderCommand.h"

namespace tzw {
	class InstancingMgr;
	class RenderQueue
	{
	public:
		RenderQueue();
		void addRenderCommand(RenderCommand& command, int level);
		void addInstancedData(InstanceRendereData & data, RenderFlag::RenderStage renderStage = RenderFlag::RenderStage::COMMON, int batchID = 0);
		std::vector<RenderCommand> & getList();
		const std::vector<RenderCommand> & getList() const;
		void clearCommands();
		void dispatch(RenderQueue * queue, uint32_t renderStage);
		void generateInstancedDrawCall(RenderFlag::RenderStage renderStage = RenderFlag::RenderStage::COMMON, int batchID = 0, int requirementArg = 0);
	private:
		std::vector<RenderCommand> m_fullList;
		InstancingMgr * m_instancesBatcher;
	};

} // namespace tzw
