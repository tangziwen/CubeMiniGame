#include "SceneView.h"

#include <cmath>
#include <cstdlib>
#include <random>

#include "3D/ShadowMap/ShadowMap.h"
#include "BackEnd/DeviceDescriptor.h"
#include "BackEnd/DeviceFrameBuffer.h"
#include "BackEnd/DeviceMaterial.h"
#include "BackEnd/DevicePipeline.h"
#include "BackEnd/DeviceRenderCommand.h"
#include "BackEnd/DeviceBuffer.h"
#include "BackEnd/RenderBackEndBase.h"
#include "Engine/DebugSystem.h"
#include "Engine/Engine.h"
#include "Interface/Drawable3D.h"
#include "Lighting/PointLight.h"
#include "RenderPath.h"
#include "RenderSettings.h"
#include "Scene/OctreeScene.h"
#include "Scene/Scene.h"
#include "Scene/SceneCuller.h"
#include "Scene/SceneMgr.h"
#include "Technique/MaterialPool.h"
#include "Texture/Texture.h"
#include "Utility/log/Log.h"

namespace tzw
{
namespace
{
struct PointLightUniform
{
	alignas(16) Matrix44 wvp;
	alignas(16) vec4 LightPos;
	alignas(16) vec4 LightColor;
};

RenderGraphResourceDesc makeGraphResourceDesc(const char* name, ImageFormat format, TextureRoleEnum role, TextureUsageEnum usage, vec2 size,
	RenderGraphResourceLayout initialColorLayout = RenderGraphResourceLayout::ColorAttachment,
	RenderGraphResourceLayout initialDepthLayout = RenderGraphResourceLayout::DepthAttachment)
{
	RenderGraphResourceDesc desc;
	desc.name = name;
	desc.format = format;
	desc.role = role;
	desc.usage = usage;
	desc.size = size;
	desc.imported = true;
	desc.initialColorLayout = initialColorLayout;
	desc.initialDepthLayout = initialDepthLayout;
	return desc;
}
}

SceneView::SceneView(RenderGraph& graph, Camera* camera, int viewIndex, vec2 size)
	: RenderView(graph, RenderViewType::Scene, viewIndex)
	, m_renderSettings(nullptr)
	, m_directLightMat(nullptr)
	, m_pointLightMat(nullptr)
	, m_skyMat(nullptr)
	, m_hbaoMat(nullptr)
	, m_ssrMat(nullptr)
	, m_fogMat(nullptr)
	, m_outputTexture(nullptr)
	, m_viewCamera(camera)
	, m_viewSize(size)
{
}

void SceneView::init()
{
	auto backEnd = Engine::shared()->getRenderBackEnd();
	auto size = m_viewSize.x > 0 && m_viewSize.y > 0 ? m_viewSize : Engine::shared()->winSize();

	DeviceAttachmentInfoList gBufferAttachments = {
		{ImageFormat::RGBA8_UNorm, false},
		{ImageFormat::RGBA8_UNorm, false},
		{ImageFormat::RGBA8_SNorm, false},
		{ImageFormat::RGBA8_UNorm, false},
		{ImageFormat::D24_UNorm_S8_UInt, true}
	};
	m_gBufferFrameBufferResource = m_renderGraph.createFrameBuffer(
		makeGraphResourceDesc("GBufferFrameBuffer", ImageFormat::RGBA8_UNorm, TextureRoleEnum::AS_COLOR, TextureUsageEnum::SAMPLE_AND_ATTACHMENT, size),
		gBufferAttachments, DeviceRenderPass::OpType::LOADCLEAR_AND_STORE, false);

	DeviceAttachmentInfoList sceneAttachments = {
		{ImageFormat::RGBA16_Float, false},
		{ImageFormat::D24_UNorm_S8_UInt, true},
	};
	m_sceneFrameBufferResource = m_renderGraph.createFrameBuffer(
		makeGraphResourceDesc("SceneFrameBuffer", ImageFormat::RGBA16_Float, TextureRoleEnum::AS_COLOR, TextureUsageEnum::SAMPLE_AND_ATTACHMENT, size),
		sceneAttachments, DeviceRenderPass::OpType::LOADCLEAR_AND_STORE, false);

	m_ssgi.init();
	m_tsaa.init();
	m_outlinePass.init();
	m_tsaaHistoryInitialized[0] = false;
	m_tsaaHistoryInitialized[1] = false;

	m_tsaaFrameBufferResources[0] = m_renderGraph.createFrameBuffer(
		makeGraphResourceDesc("TSAAFrameBufferA", ImageFormat::RGBA16_UNorm, TextureRoleEnum::AS_COLOR, TextureUsageEnum::SAMPLE_AND_ATTACHMENT, size),
		m_tsaa.attachments(), DeviceRenderPass::OpType::LOADCLEAR_AND_STORE, false);
	m_tsaaFrameBufferResources[1] = m_renderGraph.createFrameBuffer(
		makeGraphResourceDesc("TSAAFrameBufferB", ImageFormat::RGBA16_UNorm, TextureRoleEnum::AS_COLOR, TextureUsageEnum::SAMPLE_AND_ATTACHMENT, size),
		m_tsaa.attachments(), DeviceRenderPass::OpType::LOADCLEAR_AND_STORE, false);

	DeviceAttachmentInfoList hbaoAttachments = {
		{ImageFormat::RGBA16_UNorm, false},
		{ImageFormat::D24_UNorm_S8_UInt, true},
	};
	m_hbaoFrameBufferResource = m_renderGraph.createFrameBuffer(
		makeGraphResourceDesc("HBAOFrameBuffer", ImageFormat::RGBA16_UNorm, TextureRoleEnum::AS_COLOR, TextureUsageEnum::SAMPLE_AND_ATTACHMENT, size),
		hbaoAttachments, DeviceRenderPass::OpType::LOADCLEAR_AND_STORE, false);

	DeviceAttachmentInfoList outlineMaskAttachments = {
		{ImageFormat::RGBA8_UNorm, false},
		{ImageFormat::D24_UNorm_S8_UInt, true},
	};
	m_outlineMaskFrameBufferResource = m_renderGraph.createFrameBuffer(
		makeGraphResourceDesc("OutlineMaskFrameBuffer", ImageFormat::RGBA8_UNorm, TextureRoleEnum::AS_COLOR,
			TextureUsageEnum::SAMPLE_AND_ATTACHMENT, size),
		outlineMaskAttachments, DeviceRenderPass::OpType::LOADCLEAR_AND_STORE, false);
	m_outlineFrameBufferResource = m_renderGraph.createFrameBuffer(
		makeGraphResourceDesc("OutlineFrameBuffer", ImageFormat::RGBA16_UNorm, TextureRoleEnum::AS_COLOR,
			TextureUsageEnum::SAMPLE_AND_ATTACHMENT, size),
		{{ImageFormat::RGBA16_UNorm, false}}, DeviceRenderPass::OpType::LOADCLEAR_AND_STORE, false);

	m_bloom.init(m_renderGraph, size);

	if(!backEnd->isWireframeRasterModeSupported())
	{
		DebugSystem::shared()->setWireframeOverlayEnabled(false);
	}

	addSubmitDrawPass(DrawPassType::GBuffer);
	addSubmitDrawPass(DrawPassType::AfterDepthClear);
	addSubmitDrawPass(DrawPassType::Transparent);
	addSubmitDrawPass(DrawPassType::DebugLayer);

	initRenderGraphResources();
	initRenderGraphMaterials();
}

void SceneView::initRenderGraphResources()
{
	auto sceneFrameBuffer = m_sceneFrameBufferResource->get<DeviceFrameBuffer>();
	auto size = sceneFrameBuffer ? sceneFrameBuffer->getSize() : Engine::shared()->winSize();

	m_sceneColorResource = m_renderGraph.colorAttachment(m_sceneFrameBufferResource);
	m_sceneDepthResource = m_renderGraph.depthAttachment(m_sceneFrameBufferResource);
    m_sceneColorCopyResource = m_renderGraph.createTexture(
        makeGraphResourceDesc("SceneColorCopy", ImageFormat::RGBA16_Float, TextureRoleEnum::AS_COLOR,
            TextureUsageEnum::SAMPLE_AND_ATTACHMENT, size));
	for(uint32_t index = 0; index < 4; index++)
	{
		m_gBufferColorResources[index] = m_renderGraph.colorAttachment(m_gBufferFrameBufferResource, index);
	}
	m_gBufferBaseColorResource = m_gBufferColorResources[0];
	m_gBufferNormalResource = m_gBufferColorResources[2];
	m_gBufferDepthResource = m_renderGraph.depthAttachment(m_gBufferFrameBufferResource);
	m_hbaoOutputResource = m_renderGraph.colorAttachment(m_hbaoFrameBufferResource);
	m_hbaoDepthResource = m_renderGraph.depthAttachment(m_hbaoFrameBufferResource);
	m_outlineMaskColorResource = m_renderGraph.colorAttachment(m_outlineMaskFrameBufferResource);
	m_outlineMaskDepthResource = m_renderGraph.depthAttachment(m_outlineMaskFrameBufferResource);
	m_outlineOutputResource = m_renderGraph.colorAttachment(m_outlineFrameBufferResource);
	for(int layer = 0; layer < BLOOM_LAYERS; layer++)
	{
		for(int index = 0; index < 2; index++)
		{
			m_bloomTextureResources[layer][index] = m_bloom.bloomTexture(layer, index);
		}
	}
	m_bloomBrightOutputResource = m_bloomTextureResources[0][0];
	for(int index = 0; index < 2; index++)
	{
		m_tsaaColorResources[index] = m_renderGraph.colorAttachment(m_tsaaFrameBufferResources[index]);
		m_tsaaDepthResources[index] = m_renderGraph.depthAttachment(m_tsaaFrameBufferResources[index]);
	}
}

void SceneView::initRenderGraphMaterials()
{
	if(m_directLightMat)
	{
		return;
	}
	m_directLightMat = new MaterialInstance();
	m_directLightMat->loadFromMaterial("DirectLight");

	m_pointLightMat = new MaterialInstance();
	m_pointLightMat->loadFromMaterial("PointLight");

	m_skyMat = new MaterialInstance();
	m_skyMat->loadFromMaterial("Sky");

	m_hbaoMat = new MaterialInstance();
	m_hbaoMat->loadFromMaterial("HBAO");
	static Texture * jitterTex = nullptr;
	if(!jitterTex)
	{
		std::mt19937 rmt;

		unsigned char *  jitterTexMem  = (unsigned char * )malloc(8 * 8 * 4);
		float numDir = 8.0;
		for(int w = 0; w < 8; w++)
		{
			for(int h = 0; h< 8; h++)
			{
				float Rand1 = static_cast<float>(rmt()) / 4294967296.0f;
				float Rand2 = static_cast<float>(rmt()) / 4294967296.0f;
				float Angle = 2.f * 3.14156 * Rand1 / numDir;
				int index = h * 8 * 4 + w * 4;
				float tmp = cosf(Angle);
				float tmp1 = tmp * 0.5 + 0.5;
				float tmp2 = tmp1 * 255;
				jitterTexMem[index] = (unsigned char)(tmp2);
				jitterTexMem[index + 1] = (unsigned char)((sinf(Angle) * 0.5 + 0.5) * 255);
				jitterTexMem[index + 2] = (unsigned char)(Rand2 * 255);
				jitterTexMem[index + 3] = 0;
			}
		}
		jitterTex = new Texture(jitterTexMem, 8, 8, ImageFormat::RGBA8_UNorm);
	}
	m_hbaoMat->setTex("jitterTex", jitterTex);
	MaterialPool::shared()->addMaterial("HBAO", m_hbaoMat);

	m_ssrMat = new MaterialInstance();
	m_ssrMat->loadFromMaterial("SSR");
	MaterialPool::shared()->addMaterial("SSR", m_ssrMat);

	m_fogMat = new MaterialInstance();
	m_fogMat->loadFromMaterial("GlobalFog");
	MaterialPool::shared()->addMaterial("GlobalFog", m_fogMat);
}

RenderGraphNode SceneView::buildRenderGraph()
{
	RenderSettings defaultSettings;
	const auto& settings = m_renderSettings ? *m_renderSettings : defaultSettings;
	m_outputTexture = nullptr;
	m_shadowTextureResources.clear();
	for(const auto& node : m_shadowNodes)
	{
		m_shadowTextureResources.emplace_back(node.output("depth"));
	}

	DeviceAttachmentInfoList sceneLoadAttachments = {
		{ImageFormat::RGBA16_Float, false},
		{ImageFormat::D24_UNorm_S8_UInt, true},
	};
	auto readGBuffer = [this](RenderGraphPassDesc& accesses)
	{
		for(auto resource : m_gBufferColorResources)
		{
			accesses.readColor(resource);
		}
		accesses.readDepth(m_gBufferDepthResource);
	};
	auto writeGBuffer = [this](RenderGraphPassDesc& accesses)
	{
		for(auto resource : m_gBufferColorResources)
		{
			accesses.writeColor(resource, RenderGraphResourceLayout::ShaderRead);
		}
		accesses.writeDepth(m_gBufferDepthResource, RenderGraphResourceLayout::DepthRead);
	};
	auto readWriteScene = [this](RenderGraphPassDesc& accesses)
	{
		accesses.readWriteColor(m_sceneColorResource);
		accesses.readWriteDepth(m_sceneDepthResource);
	};

	RenderGraphRasterPassDesc gBufferPass;
	gBufferPass.camera = camera();
	gBufferPass.name = "GBufferPass";
	gBufferPass.frameBufferResource = m_gBufferFrameBufferResource;
	gBufferPass.drawPassMask = DrawPassType::GBuffer;
	gBufferPass.sceneQueue = renderQueue();
	gBufferPass.consumesSceneQueue = true;
	{
		RenderGraphPassDesc accesses;
		writeGBuffer(accesses);
		gBufferPass.resourceAccesses = accesses.resourceAccesses;
	}
	auto gBufferNode = m_renderGraph.addRasterNode(gBufferPass, [](RenderGraphPassContext& graphContext)
	{
		graphContext.drawSceneQueue();
	}).withOutput(m_gBufferBaseColorResource)
		.withOutput("color", m_gBufferBaseColorResource)
		.withOutput("depth", m_gBufferDepthResource);

	RenderGraphRasterPassDesc deferredLightingPass;
	deferredLightingPass.camera = camera();
	deferredLightingPass.name = "Deferred Sun Lighting Stage";
	deferredLightingPass.frameBufferResource = m_sceneFrameBufferResource;
	deferredLightingPass.material = m_directLightMat;
	{
		RenderGraphPassDesc accesses;
		readGBuffer(accesses);
		for(auto shadowResource : m_shadowTextureResources)
		{
			accesses.readDepth(shadowResource);
		}
		accesses.writeColor(m_sceneColorResource)
			.writeDepth(m_sceneDepthResource);
		deferredLightingPass.resourceAccesses = accesses.resourceAccesses;
	}
	auto deferredLightingNode = m_renderGraph.addFullscreenNode(deferredLightingPass, [this](RenderGraphPassContext& graphContext)
	{
		executeDeferredLightingPass(graphContext);
	}).withOutput(m_sceneColorResource)
		.withOutput("sceneColor", m_sceneColorResource);
	for(const auto& shadow : m_shadowNodes) deferredLightingNode.dependsOn(shadow);
	auto sceneChainNode = gBufferNode.connect(deferredLightingNode);

	RenderGraphRasterPassDesc pointLightingPass;
	pointLightingPass.camera = camera();
	pointLightingPass.name = "Deferred Point Light Stage";
	pointLightingPass.attachments = sceneLoadAttachments;
	pointLightingPass.opType = DeviceRenderPass::OpType::LOAD_AND_STORE;
	pointLightingPass.frameBufferResource = m_sceneFrameBufferResource;
	pointLightingPass.material = m_pointLightMat;
	{
		RenderGraphPassDesc accesses;
		readGBuffer(accesses);
		readWriteScene(accesses);
		pointLightingPass.resourceAccesses = accesses.resourceAccesses;
	}
	auto pointLightingNode = m_renderGraph.addRasterNode(pointLightingPass, [this](RenderGraphPassContext& graphContext)
	{
		executePointLightingPass(graphContext);
	}).withOutput(m_sceneColorResource)
		.withOutput("sceneColor", m_sceneColorResource);
	auto deferredSceneNode = sceneChainNode;

	RenderGraphRasterPassDesc afterDepthClearPass;
	afterDepthClearPass.camera = camera();
	afterDepthClearPass.name = "AfterDepthClearPass";
	afterDepthClearPass.attachments = sceneLoadAttachments;
	afterDepthClearPass.opType = DeviceRenderPass::OpType::LOAD_AND_STORE;
	afterDepthClearPass.frameBufferResource = m_sceneFrameBufferResource;
	afterDepthClearPass.drawPassMask = DrawPassType::AfterDepthClear;
	afterDepthClearPass.sceneQueue = renderQueue();
	afterDepthClearPass.consumesSceneQueue = true;
	{
		RenderGraphPassDesc accesses;
		readWriteScene(accesses);
		afterDepthClearPass.resourceAccesses = accesses.resourceAccesses;
	}
	auto afterDepthClearNode = m_renderGraph.addRasterNode(afterDepthClearPass, [](RenderGraphPassContext& graphContext)
	{
		graphContext.drawSceneQueue();
	}).withOutput(m_sceneColorResource)
		.withOutput("sceneColor", m_sceneColorResource)
		.dependsOn(deferredSceneNode);

	sceneChainNode = afterDepthClearNode.connect(pointLightingNode);

	RenderGraphRasterPassDesc transparentPass;
	transparentPass.camera = camera();
	transparentPass.name = "TransparentPass";
	transparentPass.attachments = sceneLoadAttachments;
	transparentPass.opType = DeviceRenderPass::OpType::LOAD_AND_STORE;
	transparentPass.frameBufferResource = m_sceneFrameBufferResource;
	transparentPass.drawPassMask = DrawPassType::Transparent;
	transparentPass.sceneQueue = renderQueue();
	transparentPass.consumesSceneQueue = true;
	{
		RenderGraphPassDesc accesses;
		readWriteScene(accesses);
		transparentPass.resourceAccesses = accesses.resourceAccesses;
	}
	auto transparentNode = m_renderGraph.addRasterNode(transparentPass, [](RenderGraphPassContext& graphContext)
	{
		graphContext.drawSceneQueue();
	}).withOutput(m_sceneColorResource)
		.withOutput("sceneColor", m_sceneColorResource);
	sceneChainNode = sceneChainNode.connect(transparentNode);

	RenderGraphRasterPassDesc skyPass;
	skyPass.camera = camera();
	skyPass.name = "Sky Stage";
	skyPass.attachments = sceneLoadAttachments;
	skyPass.opType = DeviceRenderPass::OpType::LOAD_AND_STORE;
	skyPass.frameBufferResource = m_sceneFrameBufferResource;
	skyPass.material = m_skyMat;
	{
		RenderGraphPassDesc accesses;
		accesses.readDepth(m_gBufferDepthResource)
			.readWriteColor(m_sceneColorResource)
			.readWriteDepth(m_sceneDepthResource);
		skyPass.resourceAccesses = accesses.resourceAccesses;
	}
	auto skyNode = m_renderGraph.addRasterNode(skyPass, [this](RenderGraphPassContext& graphContext)
	{
		executeSkyPass(graphContext);
	}).withOutput(m_sceneColorResource)
		.withOutput("sceneColor", m_sceneColorResource);
	sceneChainNode = sceneChainNode.connect(skyNode);

	RenderGraphRasterPassDesc debugWireframePass;
	debugWireframePass.camera = camera();
	debugWireframePass.name = "Debug Wireframe Pass";
	debugWireframePass.attachments = sceneLoadAttachments;
	debugWireframePass.opType = DeviceRenderPass::OpType::LOAD_AND_STORE;
	debugWireframePass.frameBufferResource = m_sceneFrameBufferResource;
	debugWireframePass.drawPassMask = DrawPassType::DebugLayer;
	debugWireframePass.sceneQueue = renderQueue();
	debugWireframePass.consumesSceneQueue = true;
	{
		RenderGraphPassDesc accesses;
		readWriteScene(accesses);
		debugWireframePass.resourceAccesses = accesses.resourceAccesses;
	}
	auto debugWireframeNode = m_renderGraph.addRasterNode(debugWireframePass, [this](RenderGraphPassContext& graphContext)
	{
		executeDebugWireframePass(graphContext);
	}).withOutput(m_sceneColorResource)
		.withOutput("sceneColor", m_sceneColorResource);
	sceneChainNode = sceneChainNode.connect(debugWireframeNode);

	const bool needsSceneColorCopy = settings.ssrEnabled() || settings.ssgiEnabled();
	RenderGraphNode hbaoNode;
	if(settings.ssrEnabled())
	{
		RenderGraphRasterPassDesc hbaoPass;
		hbaoPass.camera = camera();
		hbaoPass.name = "HBAO";
		hbaoPass.frameBufferResource = m_hbaoFrameBufferResource;
		hbaoPass.material = m_hbaoMat;
		{
			RenderGraphPassDesc accesses;
			readGBuffer(accesses);
			accesses.writeColor(m_hbaoOutputResource, RenderGraphResourceLayout::ShaderRead)
				.writeDepth(m_hbaoDepthResource);
			hbaoPass.resourceAccesses = accesses.resourceAccesses;
		}
		hbaoNode = m_renderGraph.addFullscreenNode(hbaoPass, [this](RenderGraphPassContext& graphContext)
		{
			executeHBAOPass(graphContext);
		}).withOutput(m_hbaoOutputResource)
			.withOutput("color", m_hbaoOutputResource)
			.dependsOn(gBufferNode);
	}

	RenderGraphNode sceneColorCopyNode;
	if(needsSceneColorCopy)
	{
		sceneColorCopyNode = m_renderGraph.addBlitNode("SceneColorCopy", m_sceneColorResource, m_sceneColorCopyResource)
			.withOutput("sceneColor", m_sceneColorCopyResource)
			.dependsOn(sceneChainNode);
	}

	if(settings.ssrEnabled())
	{
		RenderGraphRasterPassDesc ssrPass;
		ssrPass.camera = camera();
		ssrPass.name = "SSR";
		ssrPass.attachments = sceneLoadAttachments;
		ssrPass.opType = DeviceRenderPass::OpType::LOAD_AND_STORE;
		ssrPass.frameBufferResource = m_sceneFrameBufferResource;
		ssrPass.material = m_ssrMat;
		{
			RenderGraphPassDesc accesses;
			readGBuffer(accesses);
			accesses.readColor(m_sceneColorCopyResource)
				.readColor(m_hbaoOutputResource)
				.readWriteColor(m_sceneColorResource)
				.readWriteDepth(m_sceneDepthResource);
			ssrPass.resourceAccesses = accesses.resourceAccesses;
		}
		auto ssrNode = m_renderGraph.addFullscreenNode(ssrPass, [this](RenderGraphPassContext& graphContext)
		{
			executeSSRPass(graphContext);
		}).withOutput(m_sceneColorResource)
			.withOutput("sceneColor", m_sceneColorResource)
			.dependsOn(sceneChainNode)
			.dependsOn(hbaoNode)
			.dependsOn(sceneColorCopyNode);
		sceneChainNode = ssrNode;
	}

	if(settings.ssgiEnabled())
	{
		RenderGraphRasterPassDesc ssgiPass;
		ssgiPass.camera = camera();
		ssgiPass.name = "SSGI";
		ssgiPass.attachments = sceneLoadAttachments;
		ssgiPass.opType = DeviceRenderPass::OpType::LOAD_AND_STORE;
		ssgiPass.frameBufferResource = m_sceneFrameBufferResource;
		ssgiPass.material = m_ssgi.material();
		{
			RenderGraphPassDesc accesses;
			accesses.readColor(m_sceneColorCopyResource)
			.readDepth(m_gBufferDepthResource)
			.readColor(m_gBufferNormalResource)
			.readColor(m_gBufferBaseColorResource)
			.readWriteColor(m_sceneColorResource)
			.readWriteDepth(m_sceneDepthResource);
			ssgiPass.resourceAccesses = accesses.resourceAccesses;
		}
		auto ssgiNode = m_renderGraph.addFullscreenNode(ssgiPass, [this](RenderGraphPassContext& graphContext)
		{
			executeSSGIPass(graphContext);
		})
			.withOutput(m_sceneColorResource)
			.withOutput("sceneColor", m_sceneColorResource)
			.dependsOn(sceneChainNode)
			.dependsOn(sceneColorCopyNode);
		sceneChainNode = ssgiNode;
	}

	if(settings.fogEnabled())
	{
		RenderGraphRasterPassDesc fogPass;
		fogPass.camera = camera();
		fogPass.name = "Fog";
		fogPass.attachments = sceneLoadAttachments;
		fogPass.opType = DeviceRenderPass::OpType::LOAD_AND_STORE;
		fogPass.frameBufferResource = m_sceneFrameBufferResource;
		fogPass.material = m_fogMat;
		{
			RenderGraphPassDesc accesses;
			readGBuffer(accesses);
			readWriteScene(accesses);
			fogPass.resourceAccesses = accesses.resourceAccesses;
		}
		auto fogNode = m_renderGraph.addFullscreenNode(fogPass, [this](RenderGraphPassContext& graphContext)
		{
			executeFogPass(graphContext);
		}).withOutput(m_sceneColorResource)
			.withOutput("sceneColor", m_sceneColorResource)
			.dependsOn(sceneChainNode);
		sceneChainNode = fogNode;
	}

	if(settings.bloomEnabled())
	{
		RenderGraphComputePassDesc brightPass;
		brightPass.name = "Bright Pass";
		brightPass.shaderCollection = m_bloom.brightShader();
		{
			RenderGraphPassDesc accesses;
			accesses.readStorageImage(m_sceneColorResource)
				.writeStorageImage(m_bloomBrightOutputResource);
			brightPass.resourceAccesses = accesses.resourceAccesses;
		}
		auto brightNode = m_renderGraph.addComputeNode(brightPass, [this](RenderGraphPassContext& graphContext)
		{
			m_bloom.executeBrightPass(graphContext, m_sceneColorResource);
		}).withOutput(m_bloomBrightOutputResource)
			.withOutput("color", m_bloomBrightOutputResource)
			.dependsOn(sceneChainNode);

		auto downSampleNode = brightNode;
		for(int layer = 0; layer < BLOOM_LAYERS - 1; layer++)
		{
			RenderGraphComputePassDesc downSamplePass;
			downSamplePass.name = "Bloom Downsample " + std::to_string(layer);
			downSamplePass.shaderCollection = m_bloom.downSampleShader();
			RenderGraphPassDesc accesses;
			accesses.readStorageImage(m_bloomTextureResources[layer][0])
				.writeStorageImage(m_bloomTextureResources[layer + 1][0]);
			downSamplePass.resourceAccesses = accesses.resourceAccesses;
			downSampleNode = m_renderGraph.addComputeNode(downSamplePass, [this, layer](RenderGraphPassContext& graphContext)
			{
				m_bloom.executeDownSamplePass(graphContext, layer);
			}).withOutput(m_bloomTextureResources[layer + 1][0])
				.dependsOn(downSampleNode);
		}

		RenderGraphNode blurNodes[BLOOM_LAYERS];
		for(int layer = 0; layer < BLOOM_LAYERS; layer++)
		{
			RenderGraphComputePassDesc blurVerticalPass;
			blurVerticalPass.name = "Bloom Blur Vertical " + std::to_string(layer);
			blurVerticalPass.shaderCollection = m_bloom.blurShader(0);
			RenderGraphPassDesc verticalAccesses;
			verticalAccesses.readStorageImage(m_bloomTextureResources[layer][0])
				.writeStorageImage(m_bloomTextureResources[layer][1]);
			blurVerticalPass.resourceAccesses = verticalAccesses.resourceAccesses;
			auto verticalNode = m_renderGraph.addComputeNode(blurVerticalPass, [this, layer](RenderGraphPassContext& graphContext)
			{
				m_bloom.executeBlurPass(graphContext, layer, 0);
			}).withOutput(m_bloomTextureResources[layer][1])
				.dependsOn(downSampleNode);

			RenderGraphComputePassDesc blurHorizontalPass;
			blurHorizontalPass.name = "Bloom Blur Horizontal " + std::to_string(layer);
			blurHorizontalPass.shaderCollection = m_bloom.blurShader(1);
			RenderGraphPassDesc horizontalAccesses;
			horizontalAccesses.readStorageImage(m_bloomTextureResources[layer][1])
				.writeStorageImage(m_bloomTextureResources[layer][0], RenderGraphResourceLayout::ShaderRead);
			blurHorizontalPass.resourceAccesses = horizontalAccesses.resourceAccesses;
			blurNodes[layer] = m_renderGraph.addComputeNode(blurHorizontalPass, [this, layer](RenderGraphPassContext& graphContext)
			{
				m_bloom.executeBlurPass(graphContext, layer, 1);
			}).withOutput(m_bloomTextureResources[layer][0])
				.dependsOn(verticalNode);
		}

		RenderGraphRasterPassDesc bloomCompositePass;
		bloomCompositePass.camera = camera();
		bloomCompositePass.name = "Bloom Composite";
		bloomCompositePass.attachments = sceneLoadAttachments;
		bloomCompositePass.opType = DeviceRenderPass::OpType::LOAD_AND_STORE;
		bloomCompositePass.frameBufferResource = m_sceneFrameBufferResource;
		bloomCompositePass.material = m_bloom.compositeMaterial();
		RenderGraphPassDesc compositeAccesses;
		readWriteScene(compositeAccesses);
		for(int layer = 0; layer < BLOOM_LAYERS; layer++)
		{
			compositeAccesses.readColor(m_bloomTextureResources[layer][0]);
		}
		bloomCompositePass.resourceAccesses = compositeAccesses.resourceAccesses;
		auto bloomNode = m_renderGraph.addFullscreenNode(bloomCompositePass, [this](RenderGraphPassContext& graphContext)
		{
			m_bloom.executeCompositePass(graphContext);
		})
			.withOutput(m_sceneColorResource)
			.withOutput("sceneColor", m_sceneColorResource);
		for(auto& blurNode : blurNodes)
		{
			bloomNode = bloomNode.dependsOn(blurNode);
		}
		sceneChainNode = bloomNode;
	}

	RenderGraphNode outputNode;
	if(settings.aaEnabled())
	{
		auto targetIndex = m_tsaa.targetBufferIndex();
		auto historyIndex = m_tsaa.historyBufferIndex();
		auto tsaaTargetResource = m_tsaaFrameBufferResources[targetIndex];
		auto tsaaHistoryColor = m_tsaaColorResources[historyIndex];
		RenderGraphNode historyClearNode;
		if(!m_tsaaHistoryInitialized[historyIndex])
		{
			RenderGraphRasterPassDesc historyClearPass;
			historyClearPass.camera = camera();
			historyClearPass.name = "TSAA History Clear " + std::to_string(historyIndex);
			historyClearPass.frameBufferResource = m_tsaaFrameBufferResources[historyIndex];
			RenderGraphPassDesc accesses;
			accesses.writeColor(tsaaHistoryColor, RenderGraphResourceLayout::ShaderRead)
				.writeDepth(m_tsaaDepthResources[historyIndex], RenderGraphResourceLayout::DepthRead);
			historyClearPass.resourceAccesses = accesses.resourceAccesses;
			historyClearNode = m_renderGraph.addRasterNode(historyClearPass)
				.withOutput(tsaaHistoryColor);
		}
		RenderGraphRasterPassDesc tsaaPass;
		tsaaPass.camera = camera();
		tsaaPass.name = "TSAA";
		tsaaPass.frameBufferResource = tsaaTargetResource;
		tsaaPass.material = m_tsaa.material();
		{
			RenderGraphPassDesc accesses;
			accesses.readColor(m_sceneColorResource)
				.readColor(tsaaHistoryColor)
				.readDepth(m_tsaaDepthResources[historyIndex])
				.readDepth(m_gBufferDepthResource)
				.writeColor(m_tsaaColorResources[targetIndex], RenderGraphResourceLayout::ShaderRead)
				.writeDepth(m_tsaaDepthResources[targetIndex], RenderGraphResourceLayout::DepthRead);
			tsaaPass.resourceAccesses = accesses.resourceAccesses;
		}
		outputNode = m_renderGraph.addFullscreenNode(tsaaPass, [this](RenderGraphPassContext& graphContext)
		{
			executeTSAAPass(graphContext);
		}).withOutput(m_tsaaColorResources[targetIndex])
			.withOutput("color", m_tsaaColorResources[targetIndex])
			.withOutput("history", tsaaHistoryColor)
			.dependsOn(sceneChainNode);
		if(historyClearNode.isValid())
		{
			outputNode = outputNode.dependsOn(historyClearNode);
		}
	}
	else
	{
		outputNode = sceneChainNode.withOutput(m_sceneColorResource)
			.withOutput("sceneColor", m_sceneColorResource);
	}

	auto finalOutputNode = outputNode;
	auto finalSceneResource = outputNode.output();
	if(m_outlinePass.hasOutlineCommands(renderQueue()) && m_outlineOutputResource.isValid())
	{
		RenderGraphRasterPassDesc outlineMaskPass;
		outlineMaskPass.camera = camera();
		outlineMaskPass.name = "Outline Mask";
		outlineMaskPass.frameBufferResource = m_outlineMaskFrameBufferResource;
		outlineMaskPass.drawPassMask = DrawPassType::OutlineMask;
		outlineMaskPass.sceneQueue = renderQueue();
		RenderGraphPassDesc maskAccesses;
		maskAccesses.writeColor(m_outlineMaskColorResource, RenderGraphResourceLayout::ShaderRead)
			.writeDepth(m_outlineMaskDepthResource, RenderGraphResourceLayout::DepthRead);
		outlineMaskPass.resourceAccesses = maskAccesses.resourceAccesses;
		auto outlineMaskNode = m_renderGraph.addRasterNode(outlineMaskPass, [this](RenderGraphPassContext& graphContext)
		{
			m_outlinePass.executeMask(graphContext);
		}).withOutput(m_outlineMaskColorResource)
			.dependsOn(gBufferNode);

		RenderGraphRasterPassDesc outlineCompositePass;
		outlineCompositePass.camera = camera();
		outlineCompositePass.name = "Outline Composite";
		outlineCompositePass.frameBufferResource = m_outlineFrameBufferResource;
		outlineCompositePass.material = m_outlinePass.compositeMaterial();
		RenderGraphPassDesc outlineAccesses;
		outlineAccesses.readColor(finalSceneResource)
			.readColor(m_outlineMaskColorResource)
			.readDepth(m_outlineMaskDepthResource)
			.readDepth(m_gBufferDepthResource)
			.writeColor(m_outlineOutputResource, RenderGraphResourceLayout::ShaderRead);
		outlineCompositePass.resourceAccesses = outlineAccesses.resourceAccesses;
		finalOutputNode = m_renderGraph.addFullscreenNode(outlineCompositePass,
			[this, finalSceneResource](RenderGraphPassContext& graphContext)
		{
			m_outlinePass.executeComposite(graphContext,
				finalSceneResource->get<DeviceTexture>(),
				m_outlineMaskColorResource->get<DeviceTexture>(),
				m_outlineMaskDepthResource->get<DeviceTexture>(),
				m_gBufferDepthResource->get<DeviceTexture>());
		})
			.withOutput(m_outlineOutputResource)
			.withOutput("sceneColor", m_outlineOutputResource)
			.dependsOn(outputNode)
			.dependsOn(outlineMaskNode);
		finalSceneResource = finalOutputNode.output();
	}
	m_outputTexture = finalSceneResource->get<DeviceTexture>();

	return finalOutputNode;
}

size_t SceneView::bindGBufferTextures(DeviceDescriptor* descriptor, int firstBinding) const
{
	auto gBufferFrameBuffer = m_gBufferFrameBufferResource->get<DeviceFrameBuffer>();
	if(!descriptor || !gBufferFrameBuffer)
	{
		return 0;
	}

	auto& gbufferTex = gBufferFrameBuffer->getTextureList();
	for(size_t i = 0; i < gbufferTex.size(); i++)
	{
		descriptor->updateDescriptorByBinding(static_cast<int>(i) + firstBinding, gbufferTex[i]);
	}
	return gbufferTex.size();
}

void SceneView::collect()
{
	auto currScene = g_GetCurrScene();
	setCamera(m_viewCamera ? m_viewCamera : (currScene ? currScene->defaultCamera() : nullptr));
	SceneCuller::shared()->collect(this);
	applyCameraToCommands(camera());
}

void SceneView::onGraphExecuted()
{
	RenderSettings defaultSettings;
	const auto& settings = m_renderSettings ? *m_renderSettings : defaultSettings;
	if(settings.aaEnabled())
	{
		m_tsaaHistoryInitialized[m_tsaa.targetBufferIndex()] = true;
		m_tsaaHistoryInitialized[m_tsaa.historyBufferIndex()] = true;
	}
}

void SceneView::executeDeferredLightingPass(RenderGraphPassContext& graphContext)
{
	auto material = graphContext.material();
	auto descriptorSet = graphContext.materialDescriptor();
	if(!material || !descriptorSet)
	{
		return;
	}

	Matrix44 lightVPList[SHADOWMAP_CASCADE_NUM] = {};
	float shadowEnd[SHADOWMAP_CASCADE_NUM] = {};
	for(int i = 0; i < SHADOWMAP_CASCADE_NUM; i++)
	{
		shadowEnd[i] = ShadowMap::shared()->getCascadeEnd(i);
		lightVPList[i] = ShadowMap::shared()->getLightProjectionMatrix(i) * ShadowMap::shared()->getLightViewMatrix();
	}
	material->updateUniformSingle("TU_LightVP", lightVPList, sizeof(lightVPList));
	material->updateUniformSingle("TU_ShadowMapEnd", shadowEnd, sizeof(shadowEnd));

	bindGBufferTextures(descriptorSet, 1);
	graphContext.bindTextures(8, m_shadowTextureResources);
	graphContext.bindSinglePipelineDescriptor();
	graphContext.drawScreenQuad();
}

void SceneView::executePointLightingPass(RenderGraphPassContext& graphContext)
{
	bindGBufferTextures(graphContext.materialDescriptor(), 1);

	std::vector<Drawable3D *> pointlightList;
	auto currScene = g_GetCurrScene();
	if(!currScene)
	{
		return;
	}
	currScene->getOctreeScene()->cullingByCameraExtraFlag(camera(), static_cast<uint32_t>(DrawableFlag::PointLight), DrawPassType::All, pointlightList);
	for(auto obj : pointlightList)
	{
		PointLight* p = static_cast<PointLight*>(obj);
		if(!p->getIsVisible())
		{
			continue;
		}

		PointLightUniform uniform;
		auto projection = camera()->projection();
		Matrix44 m;
		m.setToIdentity();
		m.setTranslate(p->getWorldPos());
		auto r = p->getRadius();
		m.setScale(vec3(r, r, r));
		auto v = camera()->getViewMatrix();
		uniform.wvp = projection * v * m;
		uniform.LightPos = vec4(p->getWorldPos(), p->getRadius());
		uniform.LightColor = vec4(p->getLightColor() * p->intensity(), 1);
		if(!graphContext.bindItemUniform(0, &uniform, sizeof(PointLightUniform))) return;
		graphContext.drawSphere();
	}
}

void SceneView::executeSkyPass(RenderGraphPassContext& graphContext)
{
	auto descriptorSet = graphContext.materialDescriptor();
	auto gBufferDepth = m_gBufferDepthResource->get<DeviceTexture>();
	if(!descriptorSet || !gBufferDepth)
	{
		return;
	}

	Matrix44 scale;
	scale.setScale(vec3(6360000.0f, 6360000.0f, 6360000.0f));
	Matrix44 m = camera()->getViewProjectionMatrix() * scale;
	descriptorSet->updateDescriptorByBinding(1, gBufferDepth);
	if(!graphContext.bindItemUniform(0, &m, sizeof(Matrix44))) return;
	graphContext.drawSphere();
}

void SceneView::executeDebugWireframePass(RenderGraphPassContext& graphContext)
{
	if(!DebugSystem::shared()->isWireframeOverlayEnabled())
	{
		return;
	}
	auto debugQueue = DebugSystem::shared()->buildWireframeQueue(graphContext.sceneQueue());
	if(debugQueue && !debugQueue->getList().empty())
	{
		graphContext.drawQueue(debugQueue);
	}
}

void SceneView::executeHBAOPass(RenderGraphPassContext& graphContext)
{
	auto pipeline = graphContext.pipeline();
	auto descriptorSet = graphContext.materialDescriptor();
	if(!pipeline || !descriptorSet)
	{
		return;
	}

	Matrix44 proj = camera()->projection();
	const float* P = proj.data();
	float R = 0.8;
	pipeline->getMat()->setVar("TU_RadiusInfo", vec4(R, R * R, tanf(camera()->getFov() * 0.5f* 3.14 / 180.0), 0.0));
	vec4 projInfoPerspective = vec4(
		2.0f / (P[4 * 0 + 0]),
		2.0f / (P[4 * 1 + 1]),
		-(1.0f - P[4 * 2 + 0]) / P[4 * 0 + 0],
		-(1.0f + P[4 * 2 + 1]) / P[4 * 1 + 1]
	);
	pipeline->getMat()->setVar("TU_ProjInfo", projInfoPerspective);
	bindGBufferTextures(descriptorSet, 1);

	Matrix44 m = camera()->getViewProjectionMatrix();
	if(!graphContext.bindItemUniform(0, &m, sizeof(Matrix44))) return;
	graphContext.drawScreenQuad();
}

void SceneView::executeSSRPass(RenderGraphPassContext& graphContext)
{
	auto sceneColorCopy = m_sceneColorCopyResource->get<DeviceTexture>();
	auto hbaoOutput = m_hbaoOutputResource->get<DeviceTexture>();
	auto descriptorSet = graphContext.materialDescriptor();
	if(!sceneColorCopy || !hbaoOutput || !descriptorSet)
	{
		return;
	}

	auto gbufferTextureCount = bindGBufferTextures(descriptorSet, 1);
	descriptorSet->updateDescriptorByBinding(static_cast<int>(gbufferTextureCount) + 1, sceneColorCopy);
	descriptorSet->updateDescriptorByBinding(static_cast<int>(gbufferTextureCount) + 2, hbaoOutput);

	Matrix44 m = camera()->getViewProjectionMatrix();
	if(!graphContext.bindItemUniform(0, &m, sizeof(Matrix44))) return;
	graphContext.drawScreenQuad();
}

void SceneView::executeSSGIPass(RenderGraphPassContext& graphContext)
{
	auto sceneColorCopy = m_sceneColorCopyResource->get<DeviceTexture>();
	auto gBufferDepth = m_gBufferDepthResource->get<DeviceTexture>();
	auto gBufferNormal = m_gBufferNormalResource->get<DeviceTexture>();
	auto gBufferBaseColor = m_gBufferBaseColorResource->get<DeviceTexture>();
	if(!sceneColorCopy || !gBufferDepth || !gBufferNormal || !gBufferBaseColor || !graphContext.targetFrameBuffer())
	{
		return;
	}
	m_ssgi.execute(graphContext, sceneColorCopy, gBufferDepth, gBufferNormal, gBufferBaseColor);
}

void SceneView::executeFogPass(RenderGraphPassContext& graphContext)
{
	auto descriptorSet = graphContext.materialDescriptor();
	if(!descriptorSet)
	{
		return;
	}

	bindGBufferTextures(descriptorSet, 1);
	graphContext.bindSinglePipelineDescriptor();
	graphContext.drawScreenQuad();
}

void SceneView::executeTSAAPass(RenderGraphPassContext& graphContext)
{
	auto historyFrame = m_tsaaColorResources[m_tsaa.historyBufferIndex()]->get<DeviceTexture>();
	auto historyDepth = m_tsaaDepthResources[m_tsaa.historyBufferIndex()]->get<DeviceTexture>();
	auto targetFrame = m_tsaaColorResources[m_tsaa.targetBufferIndex()]->get<DeviceTexture>();
	auto sceneColor = m_sceneColorResource->get<DeviceTexture>();
	auto gBufferDepth = m_gBufferDepthResource->get<DeviceTexture>();
	if(!historyFrame || !historyDepth || !targetFrame || !sceneColor || !gBufferDepth)
	{
		return;
	}
	m_tsaa.executeResolve(graphContext, historyFrame, historyDepth, sceneColor, gBufferDepth);
}

void SceneView::preTick(const RenderSettings& settings)
{
	auto scene = g_GetCurrScene();
	setCamera(m_viewCamera ? m_viewCamera : (scene ? scene->defaultCamera() : nullptr));
	if(settings.aaEnabled())
	{
		m_tsaa.preTick(camera());
	}
	else
	{
		m_tsaa.resetHistory();
		if(camera()) camera()->setOffsetPixel(0, 0);
	}
	if(settings.ssgiEnabled())
	{
		m_ssgi.preTick();
	}
}

void SceneView::setRenderSettings(const RenderSettings* settings)
{
	m_renderSettings = settings;
}

void SceneView::setShadowNodes(const std::vector<RenderGraphNode>& nodes)
{
	m_shadowNodes = nodes;
}

DeviceTexture* SceneView::outputTexture() const
{
	return m_outputTexture;
}

RenderQueue* SceneView::sceneQueue()
{
	return renderQueue();
}
}
