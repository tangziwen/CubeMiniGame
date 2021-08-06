#pragma once
#include "EngineSrc/BackEnd/DeviceRenderStage.h"
namespace tzw
{
	class TSAA
	{
	public:
		void init();
		DeviceRenderStage* draw(DeviceRenderCommand * cmd, DeviceTexture * currFrame, DeviceTexture * Depth);
		DeviceFrameBuffer * getOutput();
		void preTick();
	protected:
		DeviceRenderStage * m_tsaaStage;
		DeviceFrameBuffer * m_bufferA;
		DeviceFrameBuffer * m_bufferB;
		Matrix44 m_lastViewProj;
		int m_index = 0;
	};
}
