#pragma once
#include "Engine/EngineDef.h"
#include <functional>
#include <unordered_set>
#include <unordered_map>
#include "Rendering/RenderCommand.h"
#include "Rendering/RenderQueues.h"
#include <array>
namespace tzw {
	class SceneCuller:public Singleton<SceneCuller>
	{
	public:
		SceneCuller();
		void collectPrimitives();
		void clearCommands();
		RenderQueue * getRenderQueues();
		RenderQueue * getCSMQueues(int layer);
	private:
		RenderQueue * m_renderQueues;
		RenderQueue*  m_CSMQueue[3];
		void collectShadowCmd();
	
	};

} // namespace tzw
