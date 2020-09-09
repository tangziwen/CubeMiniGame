#pragma once
#include "Engine/EngineDef.h"
#include <functional>
#include <unordered_set>
#include <unordered_map>
#include "Rendering/RenderCommand.h"
namespace tzw {
	class RenderQueues
	{
	public:
		void addRenderCommand(RenderCommand& command, int level);
		void addShadowRenderCommand(RenderCommand & cmd, int level);
		std::vector<RenderCommand> & getShadowList(int index);
		std::vector<RenderCommand> & getCommonList();
		std::vector<RenderCommand> & getGUICommandList();
		std::vector<RenderCommand> & getTransparentList();
		std::vector<RenderCommand> & getAfterDepthList();
		void clearCommands();
	private:
		std::vector<RenderCommand> m_shadowList[3];
		std::vector<RenderCommand> m_commonList;
		std::vector<RenderCommand> m_guiCommandList;
		std::vector<RenderCommand> m_transparentList;
		std::vector<RenderCommand> m_afterDepthList;
	};

} // namespace tzw
