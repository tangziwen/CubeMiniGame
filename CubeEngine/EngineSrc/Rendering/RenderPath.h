#pragma once
#include "../Engine/EngineDef.h"
#include "RenderStage.h"
#include <vector>
namespace tzw
{
	class RenderPath
	{
	public:
		RenderPath();
		void prepare();
		void addRenderStage(RenderStage * stage);
		std::vector<RenderStage *> & getStages();
	private:
		std::vector<RenderStage *> m_stages;
	};


}