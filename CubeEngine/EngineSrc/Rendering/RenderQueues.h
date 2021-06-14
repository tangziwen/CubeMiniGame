#pragma once
#include "Engine/EngineDef.h"
#include <functional>
#include <unordered_set>
#include <unordered_map>
#include "Rendering/RenderCommand.h"
namespace tzw {
	class RenderQueue
	{
	public:
		void addRenderCommand(RenderCommand& command, int level);
		std::vector<RenderCommand> & getList();
		void clearCommands();
		void dispatch(RenderQueue * queue, uint32_t renderStage);
	private:
		std::vector<RenderCommand> m_fullList;
	};

} // namespace tzw
