#pragma once
#include "Math/vec2.h"
#include "RenderGraph.h"
namespace tzw
{
#define BLOOM_LAYERS (3)

	class DeviceShaderCollection;
	class RenderGraphPassContext;
	class MaterialInstance;
	class ShadingParams;
	class Bloom
	{
	public:
		void init(RenderGraph& graph, vec2 size);
		void executeBrightPass(RenderGraphPassContext& graphContext, RenderGraphResourceHandle sceneColor);
		void executeDownSamplePass(RenderGraphPassContext& graphContext, int layer);
		void executeBlurPass(RenderGraphPassContext& graphContext, int layer, int direction);
		void executeCompositePass(RenderGraphPassContext& graphContext);
		DeviceShaderCollection* brightShader() const;
		DeviceShaderCollection* downSampleShader() const;
		DeviceShaderCollection* blurShader(int direction) const;
		MaterialInstance* compositeMaterial() const;
		RenderGraphResourceHandle bloomTexture(int layer, int index) const;
		vec2 layerSize(int index) const;
	private:
		DeviceShaderCollection* m_brightShader = nullptr;
		DeviceShaderCollection* m_downSampleShader = nullptr;
		DeviceShaderCollection* m_blurShaders[2] = {};
		ShadingParams* m_brightParams = nullptr;
		ShadingParams* m_downSampleParams[BLOOM_LAYERS - 1] = {};
		ShadingParams* m_blurParams[BLOOM_LAYERS][2] = {};
		MaterialInstance* m_compositeMaterial = nullptr;
		RenderGraphResourceHandle m_bloomTexture[BLOOM_LAYERS][2] = {};
		vec2 m_size = vec2(0, 0);
	};

}
