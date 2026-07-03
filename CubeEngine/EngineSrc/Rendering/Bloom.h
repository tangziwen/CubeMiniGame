#pragma once
#include "EngineSrc/BackEnd/DeviceRenderStage.h"
namespace tzw
{
#define BLOOM_LAYERS (3)

	class DeviceShaderCollection;
	class DeviceTexture;
	class RenderPath;
	class RenderGraphPassContext;
	class ShadingParams;
	class Bloom
	{
	public:
		void init(DeviceFrameBuffer * frameBuffer);
		void draw(DeviceRenderCommand * cmd,RenderPath * path, DeviceTexture * sceneColor);
		void drawAfterBright(DeviceRenderCommand * cmd,RenderPath * path);
		void executeBrightPass(RenderGraphPassContext& graphContext, DeviceTexture* sceneColor);
		DeviceShaderCollection* brightShader() const;
		DeviceTexture* bloomTexture(int layer, int index) const;
	private:
		vec2 getLayerSize(int index);
		DeviceShaderCollection* m_brightShader = nullptr;
		ShadingParams* m_brightParams = nullptr;
		DeviceRenderStage * m_DownSampleStage[BLOOM_LAYERS - 1];
		DeviceRenderStage * m_bloomCompositeStage;
		DeviceRenderStage * m_blurStage[BLOOM_LAYERS][2];//ping pong XY
		DeviceTexture * m_bloomTexture[BLOOM_LAYERS][2];
	};

}
