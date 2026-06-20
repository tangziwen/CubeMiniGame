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
		// Previous-frame unjittered view-projection matrix for history reprojection.
		Matrix44 m_lastViewProj;
		int m_index = 0;
		// Current-frame jitter in pixels, applied to scene rendering and passed as TU_jitterUV.
		vec2 m_offset;
		// TSAA resolve tuning parameters.
		float m_jitterScalePixels = 1.0f;
		vec4 m_resolveParams = vec4(0.94f, 0.10f, 0.010f, 0.05f);
		vec4 m_rejectionParams = vec4(1.00f, 0.92f, 0.0f, 0.0f);
		int m_debugMode = 0;
	};
}
