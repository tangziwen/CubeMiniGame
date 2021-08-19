#pragma once
#include "EngineSrc/BackEnd/DeviceRenderStage.h"
namespace tzw
{
	class SSGI
	{
	public:
		void init();
		DeviceRenderStage* draw(DeviceRenderCommand * cmd, DeviceTexture * currFrame, DeviceTexture * Depth, DeviceTexture * normal, DeviceTexture * baseColor, DeviceFrameBuffer * OutPutTarget);
		DeviceFrameBuffer * getOutput();
		void preTick();
	protected:
		DeviceRenderStage * m_stage;
		DeviceFrameBuffer * m_bufferA;
		DeviceFrameBuffer * m_bufferB;
		DeviceFrameBuffer * m_outputTarget;
		int m_index = 0;
	};
}
