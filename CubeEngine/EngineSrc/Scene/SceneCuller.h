#pragma once
#include "Engine/EngineDef.h"
#include <functional>
#include <unordered_set>
#include <unordered_map>
#include "Rendering/RenderCommand.h"
#include "Rendering/RenderQueues.h"
namespace tzw {
	class SceneCuller:public Singleton<SceneCuller>
	{
	public:
		SceneCuller();
		void collectPrimitives();
		void clearCommands();
		RenderQueues * getRenderQueues();
	private:
		RenderQueues * m_renderQueues;
		void collectShadowCmd();
	
	};

} // namespace tzw
