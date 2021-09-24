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
		vec2 getLayerSize(int index);
		DeviceRenderStage * m_brightStage;
		DeviceRenderStage * m_DownSampleStage[BLOOM_LAYERS - 1];
		DeviceRenderStage * m_bloomCompositeStage;
		DeviceRenderStage * m_blurStage[BLOOM_LAYERS][2];//ping pong XY
		DeviceTexture * m_bloomTexture[BLOOM_LAYERS][2];
	};

}