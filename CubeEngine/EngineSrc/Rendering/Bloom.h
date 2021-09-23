#pragma once
#include "EngineSrc/BackEnd/DeviceRenderStage.h"
namespace tzw
{
#define BLOOM_LAYERS (3)

	class RenderPath;
	class Bloom
	{
	public:
		void init(DeviceFrameBuffer * frameBuffer);
		void draw(DeviceRenderCommand * cmd,RenderPath * path, DeviceTexture * sceneColor);
	private:
		DeviceRenderStage * m_brightStage;
		DeviceRenderStage * m_DownSampleStage;
		DeviceRenderStage * m_bloomCompositeStage;
		DeviceTexture * m_bloomTexture[BLOOM_LAYERS];
	};

}