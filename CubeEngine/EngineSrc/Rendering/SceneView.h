#pragma once

#include "Bloom.h"
#include "OutlinePass.h"
#include "RenderGraph.h"
#include "Rendering/RenderView.h"
#include "SSGI.h"
#include "TSAA.h"
#include <cstddef>
#include <vector>

namespace tzw
{
class DeviceDescriptor;
class DeviceFrameBuffer;
class DeviceTexture;
class MaterialInstance;
class RenderSettings;

class SceneView : public RenderView
{
public:
	SceneView(RenderGraph& graph, Camera* camera = nullptr, int viewIndex = 0, vec2 size = vec2(0, 0));

	void init() override;
	void collect() override;
	RenderGraphNode buildRenderGraph() override;
	void onGraphExecuted();
	void preTick(const RenderSettings& settings);
	void setRenderSettings(const RenderSettings* settings);
	void setShadowNodes(const std::vector<RenderGraphNode>& nodes);

	DeviceTexture* outputTexture() const;
	RenderQueue* sceneQueue();

private:
	void initRenderGraphResources();
	void initRenderGraphMaterials();
	void executeDeferredLightingPass(RenderGraphPassContext& graphContext);
	void executePointLightingPass(RenderGraphPassContext& graphContext);
	void executeSkyPass(RenderGraphPassContext& graphContext);
	void executeDebugWireframePass(RenderGraphPassContext& graphContext);
	void executeHBAOPass(RenderGraphPassContext& graphContext);
	void executeSSRPass(RenderGraphPassContext& graphContext);
	void executeSSGIPass(RenderGraphPassContext& graphContext);
	void executeFogPass(RenderGraphPassContext& graphContext);
	void executeTSAAPass(RenderGraphPassContext& graphContext);
	size_t bindGBufferTextures(DeviceDescriptor* descriptor, int firstBinding) const;

	TSAA m_tsaa;
	SSGI m_ssgi;
	Bloom m_bloom;
	OutlinePass m_outlinePass;
	const RenderSettings* m_renderSettings;
	MaterialInstance* m_directLightMat;
	MaterialInstance* m_pointLightMat;
	MaterialInstance* m_skyMat;
	MaterialInstance* m_hbaoMat;
	MaterialInstance* m_ssrMat;
	MaterialInstance* m_fogMat;
	RenderGraphResourceHandle m_gBufferFrameBufferResource;
	RenderGraphResourceHandle m_gBufferColorResources[4];
	RenderGraphResourceHandle m_hbaoFrameBufferResource;
	RenderGraphResourceHandle m_sceneColorResource;
	RenderGraphResourceHandle m_sceneDepthResource;
	RenderGraphResourceHandle m_sceneColorCopyResource;
	RenderGraphResourceHandle m_gBufferDepthResource;
	RenderGraphResourceHandle m_gBufferNormalResource;
	RenderGraphResourceHandle m_gBufferBaseColorResource;
	RenderGraphResourceHandle m_hbaoOutputResource;
	RenderGraphResourceHandle m_hbaoDepthResource;
	RenderGraphResourceHandle m_outlineMaskFrameBufferResource;
	RenderGraphResourceHandle m_outlineMaskColorResource;
	RenderGraphResourceHandle m_outlineMaskDepthResource;
	RenderGraphResourceHandle m_outlineFrameBufferResource;
	RenderGraphResourceHandle m_outlineOutputResource;
	RenderGraphResourceHandle m_bloomBrightOutputResource;
	RenderGraphResourceHandle m_bloomTextureResources[BLOOM_LAYERS][2];
	RenderGraphResourceHandle m_sceneFrameBufferResource;
	RenderGraphResourceHandle m_tsaaFrameBufferResources[2];
	RenderGraphResourceHandle m_tsaaColorResources[2];
	RenderGraphResourceHandle m_tsaaDepthResources[2];
	DeviceTexture * m_outputTexture;
	Camera* m_viewCamera;
	vec2 m_viewSize;
	std::vector<RenderGraphNode> m_shadowNodes;
	std::vector<RenderGraphResourceHandle> m_shadowTextureResources;
	bool m_tsaaHistoryInitialized[2] = {false, false};
};
}
