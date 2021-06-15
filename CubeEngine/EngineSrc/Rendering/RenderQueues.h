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
		void addInstancedData(InstanceRendereData & data);
		std::vector<RenderCommand> & getList();
		void clearCommands();
		void dispatch(RenderQueue * queue, uint32_t renderStage);
		void generateInstancedDrawCall();
	private:
		std::vector<RenderCommand> m_fullList;
		InstancingMgr * m_instancesBatcher;
	};

} // namespace tzw
