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
	SceneView();

	void init() override;
	void collect() override;
	void draw(DeviceRenderCommand* cmd, RenderPath* renderPath) override;
	void draw(DeviceRenderCommand* cmd, RenderPath* renderPath, int imageIndex);
	void preTick(const RenderSettings& settings);
	void setRenderSettings(const RenderSettings* settings);
	void setShadowTextures(const std::vector<DeviceTexture*>& shadowTextures);

	DeviceTexture* outputTexture() const;
	RenderQueue* sceneQueue();

private:
	void initRenderGraphResources();
	void initRenderGraphMaterials();
	bool buildRenderGraph(int imageIndex);
	void executeDeferredLightingPass(RenderGraphPassContext& graphContext);
	void executePointLightingPass(RenderGraphPassContext& graphContext);
	void executeSkyPass(RenderGraphPassContext& graphContext);
	void executeDebugWireframePass(RenderGraphPassContext& graphContext);
	void executeHBAOPass(RenderGraphPassContext& graphContext);
	void executeSSRPass(RenderGraphPassContext& graphContext);
	void executeSSGIPass(RenderGraphPassContext& graphContext);
	void executeFogPass(RenderGraphPassContext& graphContext);
	void executeTSAAPass(RenderGraphPassContext& graphContext);
	void executeTextureToScreenPass(RenderGraphPassContext& graphContext);
	DeviceFrameBuffer* graphFrameBuffer(RenderGraphResourceHandle handle, const RenderGraphPassContext* graphContext = nullptr) const;
	DeviceTexture* graphTexture(RenderGraphResourceHandle handle, const RenderGraphPassContext* graphContext = nullptr) const;
	DeviceTexture* graphDepthTexture(RenderGraphResourceHandle handle, const RenderGraphPassContext* graphContext = nullptr) const;
	size_t bindGBufferTextures(DeviceDescriptor* descriptor, int firstBinding, const RenderGraphPassContext* graphContext = nullptr) const;

	TSAA m_tsaa;
	SSGI m_ssgi;
	Bloom m_bloom;
	OutlinePass m_outlinePass;
	RenderGraph m_renderGraph;
	const RenderSettings* m_renderSettings;
	MaterialInstance* m_directLightMat;
	MaterialInstance* m_pointLightMat;
	MaterialInstance* m_skyMat;
	MaterialInstance* m_hbaoMat;
	MaterialInstance* m_ssrMat;
	MaterialInstance* m_fogMat;
	MaterialInstance* m_textureToScreenMat;
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
	RenderGraphResourceHandle m_screenFrameBufferResources[2];
	RenderGraphResourceHandle m_screenColorResources[2];
	RenderGraphResourceHandle m_screenDepthResources[2];
	DeviceTexture * m_sceneCopyTex;
	DeviceTexture * m_outputTexture;
	std::vector<DeviceTexture*> m_shadowTextures;
	std::vector<RenderGraphResourceHandle> m_shadowTextureResources;
	bool m_tsaaHistoryInitialized[2] = {false, false};
};
}
