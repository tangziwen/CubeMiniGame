#pragma once
#include "../Engine/EngineDef.h"
#include "BackEnd/DeviceRenderStage.h"
#include <vector>
namespace tzw
{
	class RenderPath
	{
	public:
		RenderPath();
		void prepare();
		void addRenderStage(DeviceRenderStage * stage);
		std::vector<DeviceRenderStage *> & getStages();
	private:
		std::vector<DeviceRenderStage *> m_stages;
	};


}