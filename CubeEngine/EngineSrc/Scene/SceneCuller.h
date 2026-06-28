#pragma once
#include "Engine/EngineDef.h"
namespace tzw {
	class RenderView;
	class SceneCuller:public Singleton<SceneCuller>
	{
	public:
		SceneCuller();
		void collect(RenderView * view);
	};

} // namespace tzw
