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

class SceneView : public RenderView
{
public:
	SceneView();

	void init() override;
	void collect() override;
	void draw(DeviceRenderCommand* cmd, RenderPath* renderPath) override;
	void preTick(bool isAAEnable);
	void setAAEnabled(bool isAAEnable);
	void setShadowTextures(const std::vector<DeviceTexture*>& shadowTextures);

	DeviceTexture* outputTexture() const;
	RenderQueue* sceneQueue();

private:
	void initRenderGraphResources();
	void initRenderGraph();
	void executeDeferredLightingPass(RenderGraphPassContext& graphContext);
	void executePointLightingPass(RenderGraphPassContext& graphContext);
	void executeSkyPass(RenderGraphPassContext& graphContext);
	void executeDebugWireframePass(RenderGraphPassContext& graphContext);
	void executeHBAOPass(RenderGraphPassContext& graphContext);
	void executeSSRPass(RenderGraphPassContext& graphContext);
	void executeSSGIPass(RenderGraphPassContext& graphContext);
	void executeFogPass(RenderGraphPassContext& graphContext);
	void executeBloomPass(RenderGraphPassContext& graphContext);
	void executeTSAAPass(RenderGraphPassContext& graphContext);
	void executeOutlinePass(RenderGraphPassContext& graphContext);
	DeviceFrameBuffer* graphFrameBuffer(RenderGraphResourceHandle handle, const RenderGraphPassContext* graphContext = nullptr) const;
	DeviceTexture* graphTexture(RenderGraphResourceHandle handle, const RenderGraphPassContext* graphContext = nullptr) const;
	DeviceTexture* graphDepthTexture(RenderGraphResourceHandle handle, const RenderGraphPassContext* graphContext = nullptr) const;
	size_t bindGBufferTextures(DeviceDescriptor* descriptor, int firstBinding, const RenderGraphPassContext* graphContext = nullptr) const;

	TSAA m_tsaa;
	SSGI m_ssgi;
	Bloom m_bloom;
	OutlinePass m_outlinePass;
	RenderGraph m_renderGraph;
	RenderGraphResourceHandle m_gBufferFrameBufferResource;
	RenderGraphResourceHandle m_hbaoFrameBufferResource;
	RenderGraphResourceHandle m_sceneColorResource;
	RenderGraphResourceHandle m_sceneColorCopyResource;
	RenderGraphResourceHandle m_gBufferDepthResource;
	RenderGraphResourceHandle m_gBufferNormalResource;
	RenderGraphResourceHandle m_gBufferBaseColorResource;
	RenderGraphResourceHandle m_hbaoOutputResource;
	RenderGraphResourceHandle m_sceneFrameBufferResource;
	RenderGraphPassHandle m_gBufferPass;
	RenderGraphPassHandle m_deferredLightingPass;
	RenderGraphPassHandle m_pointLightingPass;
	RenderGraphPassHandle m_transparentPass;
	RenderGraphPassHandle m_skyPass;
	RenderGraphPassHandle m_debugWireframePass;
	RenderGraphPassHandle m_hbaoPass;
	RenderGraphPassHandle m_sceneColorCopyPass;
	RenderGraphPassHandle m_ssrPass;
	RenderGraphPassHandle m_fogPass;
	RenderGraphPassHandle m_ssgiPass;
	RenderGraphPassHandle m_bloomPass;
	RenderGraphPassHandle m_tsaaPass;
	RenderGraphPassHandle m_outlinePassHandle;
	DeviceTexture * m_sceneCopyTex;
	DeviceTexture * m_outputTexture;
	std::vector<DeviceTexture*> m_shadowTextures;
	bool m_isAAEnable;
};
}
