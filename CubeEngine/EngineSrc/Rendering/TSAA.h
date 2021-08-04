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
	protected:
		DeviceRenderStage * m_tsaaStage;
		DeviceFrameBuffer * m_bufferA;
		DeviceFrameBuffer * m_bufferB;
		Matrix44 m_lastView;
		int m_index = 0;
	};
}
