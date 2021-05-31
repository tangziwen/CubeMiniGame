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
		void prepare(DeviceRenderCommand * cmd);
		void finish();
		void addRenderStage(DeviceRenderStage * stage);
		std::vector<DeviceRenderStage *> & getStages();
		DeviceRenderCommand * getCmd();
	private:
		std::vector<DeviceRenderStage *> m_stages;
		DeviceRenderCommand * m_deviceCmd;
	};


}