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
#include "BackEnd/VkRenderBackEnd.h"
#include "BackEnd/vk/DeviceTextureVK.h"
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

SceneView::SceneView()
	: RenderView(RenderViewType::Scene)
	, m_renderSettings(nullptr)
	, m_directLightMat(nullptr)
	, m_pointLightMat(nullptr)
	, m_skyMat(nullptr)
	, m_hbaoMat(nullptr)
	, m_ssrMat(nullptr)
	, m_fogMat(nullptr)
	, m_textureToScreenMat(nullptr)
	, m_sceneCopyTex(nullptr)
	, m_outputTexture(nullptr)
{
}

void SceneView::init()
{
	auto backEnd = static_cast<VKRenderBackEnd *>(Engine::shared()->getRenderBackEnd());
	auto size = Engine::shared()->winSize();
	m_renderGraph.clearResources();

	DeviceAttachmentInfoList gBufferAttachments = {
		{ImageFormat::R8G8B8A8, false},
		{ImageFormat::R8G8B8A8, false},
		{ImageFormat::R8G8B8A8_S, false},
		{ImageFormat::R8G8B8A8, false},
		{ImageFormat::D24_S8, true}
	};
	m_gBufferFrameBufferResource = m_renderGraph.createFrameBuffer(
		makeGraphResourceDesc("GBufferFrameBuffer", ImageFormat::R8G8B8A8, TextureRoleEnum::AS_COLOR, TextureUsageEnum::SAMPLE_AND_ATTACHMENT, size,
			RenderGraphResourceLayout::ShaderRead, RenderGraphResourceLayout::DepthRead),
		gBufferAttachments, DeviceRenderPass::OpType::LOADCLEAR_AND_STORE, true);

	DeviceAttachmentInfoList sceneAttachments = {
		{ImageFormat::R16G16B16A16, false},
		{ImageFormat::D24_S8, true},
	};
	m_sceneFrameBufferResource = m_renderGraph.createFrameBuffer(
		makeGraphResourceDesc("SceneFrameBuffer", ImageFormat::R16G16B16A16, TextureRoleEnum::AS_COLOR, TextureUsageEnum::SAMPLE_AND_ATTACHMENT, size),
		sceneAttachments, DeviceRenderPass::OpType::LOADCLEAR_AND_STORE, false);
	auto deferredLightingBuffer = m_renderGraph.frameBuffer(m_sceneFrameBufferResource);

	m_ssgi.init();
	m_tsaa.init();
	m_outlinePass.init();

	m_tsaaFrameBufferResources[0] = m_renderGraph.createFrameBuffer(
		makeGraphResourceDesc("TSAAFrameBufferA", ImageFormat::R16G16B16A16, TextureRoleEnum::AS_COLOR, TextureUsageEnum::SAMPLE_AND_ATTACHMENT, size,
			RenderGraphResourceLayout::ShaderRead),
		m_tsaa.attachments(), DeviceRenderPass::OpType::LOADCLEAR_AND_STORE, true);
	m_tsaaFrameBufferResources[1] = m_renderGraph.createFrameBuffer(
		makeGraphResourceDesc("TSAAFrameBufferB", ImageFormat::R16G16B16A16, TextureRoleEnum::AS_COLOR, TextureUsageEnum::SAMPLE_AND_ATTACHMENT, size,
			RenderGraphResourceLayout::ShaderRead),
		m_tsaa.attachments(), DeviceRenderPass::OpType::LOADCLEAR_AND_STORE, true);

	DeviceAttachmentInfoList hbaoAttachments = {
		{ImageFormat::R16G16B16A16, false},
		{ImageFormat::D24_S8, true},
	};
	m_hbaoFrameBufferResource = m_renderGraph.createFrameBuffer(
		makeGraphResourceDesc("HBAOFrameBuffer", ImageFormat::R16G16B16A16, TextureRoleEnum::AS_COLOR, TextureUsageEnum::SAMPLE_AND_ATTACHMENT, size),
		hbaoAttachments, DeviceRenderPass::OpType::LOADCLEAR_AND_STORE, true);

	auto sceneCopyTex = new DeviceTextureVK();
	sceneCopyTex->initEmpty(size.x, size.y, ImageFormat::R16G16B16A16_SFLOAT,TextureRoleEnum::AS_COLOR, TextureUsageEnum::SAMPLE_AND_ATTACHMENT, 1, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	m_sceneCopyTex = sceneCopyTex;

	m_bloom.init(deferredLightingBuffer);

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
	auto sceneFrameBuffer = graphFrameBuffer(m_sceneFrameBufferResource);
	auto gBufferFrameBuffer = graphFrameBuffer(m_gBufferFrameBufferResource);
	auto size = sceneFrameBuffer ? sceneFrameBuffer->getSize() : Engine::shared()->winSize();

	m_sceneColorResource = m_sceneFrameBufferResource;
	m_sceneColorCopyResource = m_renderGraph.importTexture(
		makeGraphResourceDesc("SceneColorCopy", ImageFormat::R16G16B16A16_SFLOAT, TextureRoleEnum::AS_COLOR, TextureUsageEnum::SAMPLE_AND_ATTACHMENT, size,
			RenderGraphResourceLayout::ShaderRead),
		m_sceneCopyTex);
	m_gBufferDepthResource = m_gBufferFrameBufferResource;
	m_gBufferBaseColorResource = m_gBufferFrameBufferResource;
	if(gBufferFrameBuffer)
	{
		auto& gBufferTextures = gBufferFrameBuffer->getTextureList();
		if(gBufferTextures.size() > 2)
		{
			m_gBufferNormalResource = m_renderGraph.importTexture(
				makeGraphResourceDesc("GBufferNormal", ImageFormat::R8G8B8A8_S, TextureRoleEnum::AS_COLOR, TextureUsageEnum::SAMPLE_AND_ATTACHMENT, gBufferFrameBuffer->getSize(),
					RenderGraphResourceLayout::ShaderRead),
				gBufferTextures[2]);
		}
	}
	m_hbaoOutputResource = m_hbaoFrameBufferResource;
	auto outlineOutputTexture = m_outlinePass.outputTexture();
	m_outlineOutputResource = outlineOutputTexture
		? m_renderGraph.importTexture(
			makeGraphResourceDesc("OutlineOutput", ImageFormat::R16G16B16A16, TextureRoleEnum::AS_COLOR, TextureUsageEnum::SAMPLE_AND_ATTACHMENT, size,
				RenderGraphResourceLayout::ShaderRead),
			outlineOutputTexture)
		: RenderGraphResourceHandle::invalid();
	m_bloomBrightOutputResource = m_renderGraph.importTexture(
		makeGraphResourceDesc("BloomBrightOutput", ImageFormat::R16G16B16A16, TextureRoleEnum::AS_COLOR, TextureUsageEnum::SAMPLE_AND_ATTACHMENT, size,
			RenderGraphResourceLayout::ShaderRead),
		m_bloom.bloomTexture(0, 0));

	auto backEnd = static_cast<VKRenderBackEnd *>(Engine::shared()->getRenderBackEnd());
	for(int i = 0; i < 2; i++)
	{
		m_screenFrameBufferResources[i] = m_renderGraph.importFrameBuffer(
			makeGraphResourceDesc("ScreenFrameBuffer", ImageFormat::Surface_Format, TextureRoleEnum::AS_COLOR, TextureUsageEnum::SAMPLE_AND_ATTACHMENT, size),
			backEnd->createSwapChainFrameBuffer(i));
	}
}

void SceneView::initRenderGraphMaterials()
{
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
		jitterTex = new Texture(jitterTexMem, 8, 8, ImageFormat::R8G8B8A8);
	}
	m_hbaoMat->setTex("jitterTex", jitterTex);
	MaterialPool::shared()->addMaterial("HBAO", m_hbaoMat);

	m_ssrMat = new MaterialInstance();
	m_ssrMat->loadFromMaterial("SSR");
	MaterialPool::shared()->addMaterial("SSR", m_ssrMat);

	m_fogMat = new MaterialInstance();
	m_fogMat->loadFromMaterial("GlobalFog");
	MaterialPool::shared()->addMaterial("GlobalFog", m_fogMat);

	m_textureToScreenMat = new MaterialInstance();
	m_textureToScreenMat->loadFromMaterial("TextureToScreen");
}

bool SceneView::buildRenderGraph(int imageIndex)
{
	m_renderGraph.beginBuild();
	RenderSettings defaultSettings;
	const auto& settings = m_renderSettings ? *m_renderSettings : defaultSettings;
	m_outputTexture = nullptr;

	DeviceAttachmentInfoList sceneLoadAttachments = {
		{ImageFormat::R16G16B16A16, false},
		{ImageFormat::D24_S8, true},
	};

	RenderGraphRasterPassDesc gBufferPass;
	gBufferPass.name = "GBufferPass";
	gBufferPass.frameBufferResource = m_gBufferFrameBufferResource;
	gBufferPass.drawPassMask = DrawPassType::GBuffer;
	gBufferPass.consumesSceneQueue = true;
	{
		RenderGraphPassDesc accesses;
		accesses.writeColor(m_gBufferFrameBufferResource, RenderGraphResourceLayout::ShaderRead)
			.writeDepth(m_gBufferDepthResource, RenderGraphResourceLayout::DepthRead);
		gBufferPass.resourceAccesses = accesses.resourceAccesses;
	}
	auto gBufferNode = m_renderGraph.addRasterNode(gBufferPass, [](RenderGraphPassContext& graphContext)
	{
		graphContext.drawSceneQueue();
	}).withOutput(m_gBufferFrameBufferResource)
		.withOutput("color", m_gBufferFrameBufferResource)
		.withOutput("depth", m_gBufferDepthResource);

	RenderGraphRasterPassDesc deferredLightingPass;
	deferredLightingPass.name = "Deferred Sun Lighting Stage";
	deferredLightingPass.frameBufferResource = m_sceneFrameBufferResource;
	deferredLightingPass.material = m_directLightMat;
	{
		RenderGraphPassDesc accesses;
		accesses.readColor(m_gBufferFrameBufferResource)
			.readDepth(m_gBufferDepthResource)
			.readColor(m_gBufferNormalResource)
			.writeColor(m_sceneColorResource);
		deferredLightingPass.resourceAccesses = accesses.resourceAccesses;
	}
	auto deferredLightingNode = m_renderGraph.addFullscreenNode(deferredLightingPass, [this](RenderGraphPassContext& graphContext)
	{
		executeDeferredLightingPass(graphContext);
	}).withOutput(m_sceneColorResource)
		.withOutput("sceneColor", m_sceneColorResource);
	auto sceneChainNode = gBufferNode.connect(deferredLightingNode);

	RenderGraphRasterPassDesc pointLightingPass;
	pointLightingPass.name = "Deferred Point Light Stage";
	pointLightingPass.attachments = sceneLoadAttachments;
	pointLightingPass.opType = DeviceRenderPass::OpType::LOAD_AND_STORE;
	pointLightingPass.frameBufferResource = m_sceneFrameBufferResource;
	pointLightingPass.material = m_pointLightMat;
	{
		RenderGraphPassDesc accesses;
		accesses.readColor(m_gBufferFrameBufferResource)
			.readDepth(m_gBufferDepthResource)
			.writeColor(m_sceneColorResource);
		pointLightingPass.resourceAccesses = accesses.resourceAccesses;
	}
	auto pointLightingNode = m_renderGraph.addRasterNode(pointLightingPass, [this](RenderGraphPassContext& graphContext)
	{
		executePointLightingPass(graphContext);
	}).withOutput(m_sceneColorResource)
		.withOutput("sceneColor", m_sceneColorResource);
	auto deferredSceneNode = sceneChainNode;

	RenderGraphRasterPassDesc afterDepthClearPass;
	afterDepthClearPass.name = "AfterDepthClearPass";
	afterDepthClearPass.attachments = sceneLoadAttachments;
	afterDepthClearPass.opType = DeviceRenderPass::OpType::LOAD_AND_STORE;
	afterDepthClearPass.frameBufferResource = m_sceneFrameBufferResource;
	afterDepthClearPass.drawPassMask = DrawPassType::AfterDepthClear;
	afterDepthClearPass.consumesSceneQueue = true;
	{
		RenderGraphPassDesc accesses;
		accesses.readDepth(m_gBufferDepthResource)
			.writeColor(m_sceneColorResource);
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
	transparentPass.name = "TransparentPass";
	transparentPass.attachments = sceneLoadAttachments;
	transparentPass.opType = DeviceRenderPass::OpType::LOAD_AND_STORE;
	transparentPass.frameBufferResource = m_sceneFrameBufferResource;
	transparentPass.drawPassMask = DrawPassType::Transparent;
	transparentPass.consumesSceneQueue = true;
	{
		RenderGraphPassDesc accesses;
		accesses.writeColor(m_sceneColorResource);
		transparentPass.resourceAccesses = accesses.resourceAccesses;
	}
	auto transparentNode = m_renderGraph.addRasterNode(transparentPass, [](RenderGraphPassContext& graphContext)
	{
		graphContext.drawSceneQueue();
	}).withOutput(m_sceneColorResource)
		.withOutput("sceneColor", m_sceneColorResource);
	sceneChainNode = sceneChainNode.connect(transparentNode);

	RenderGraphRasterPassDesc skyPass;
	skyPass.name = "Sky Stage";
	skyPass.attachments = sceneLoadAttachments;
	skyPass.opType = DeviceRenderPass::OpType::LOAD_AND_STORE;
	skyPass.frameBufferResource = m_sceneFrameBufferResource;
	skyPass.material = m_skyMat;
	{
		RenderGraphPassDesc accesses;
		accesses.readDepth(m_gBufferDepthResource)
			.writeColor(m_sceneColorResource);
		skyPass.resourceAccesses = accesses.resourceAccesses;
	}
	auto skyNode = m_renderGraph.addRasterNode(skyPass, [this](RenderGraphPassContext& graphContext)
	{
		executeSkyPass(graphContext);
	}).withOutput(m_sceneColorResource)
		.withOutput("sceneColor", m_sceneColorResource);
	sceneChainNode = sceneChainNode.connect(skyNode);

	RenderGraphRasterPassDesc debugWireframePass;
	debugWireframePass.name = "Debug Wireframe Pass";
	debugWireframePass.attachments = sceneLoadAttachments;
	debugWireframePass.opType = DeviceRenderPass::OpType::LOAD_AND_STORE;
	debugWireframePass.frameBufferResource = m_sceneFrameBufferResource;
	debugWireframePass.drawPassMask = DrawPassType::DebugLayer;
	debugWireframePass.consumesSceneQueue = true;
	{
		RenderGraphPassDesc accesses;
		accesses.writeColor(m_sceneColorResource);
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
		hbaoPass.name = "HBAO";
		hbaoPass.frameBufferResource = m_hbaoFrameBufferResource;
		hbaoPass.material = m_hbaoMat;
		{
			RenderGraphPassDesc accesses;
			accesses.readColor(m_gBufferFrameBufferResource)
				.readDepth(m_gBufferDepthResource)
				.readColor(m_gBufferNormalResource)
				.writeColor(m_hbaoOutputResource, RenderGraphResourceLayout::ShaderRead);
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
		ssrPass.name = "SSR";
		ssrPass.attachments = sceneLoadAttachments;
		ssrPass.opType = DeviceRenderPass::OpType::LOAD_AND_STORE;
		ssrPass.frameBufferResource = m_sceneFrameBufferResource;
		ssrPass.material = m_ssrMat;
		{
			RenderGraphPassDesc accesses;
			accesses.readColor(m_gBufferFrameBufferResource)
				.readDepth(m_gBufferDepthResource)
				.readColor(m_gBufferNormalResource)
				.readColor(m_sceneColorCopyResource)
				.readColor(m_hbaoOutputResource)
				.writeColor(m_sceneColorResource);
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
		RenderGraphPassDesc ssgiPass;
		ssgiPass.name = "SSGI";
		ssgiPass.readColor(m_sceneColorCopyResource)
			.readDepth(m_gBufferDepthResource)
			.readColor(m_gBufferNormalResource)
			.readColor(m_gBufferBaseColorResource)
			.writeColor(m_sceneColorResource);
		ssgiPass.execute = [this](RenderGraphPassContext& graphContext)
		{
			executeSSGIPass(graphContext);
		};
		auto ssgiNode = m_renderGraph.addExternalNode(ssgiPass)
			.withOutput(m_sceneColorResource)
			.withOutput("sceneColor", m_sceneColorResource)
			.dependsOn(sceneChainNode)
			.dependsOn(sceneColorCopyNode);
		sceneChainNode = ssgiNode;
	}

	if(settings.fogEnabled())
	{
		RenderGraphRasterPassDesc fogPass;
		fogPass.name = "Fog";
		fogPass.attachments = sceneLoadAttachments;
		fogPass.opType = DeviceRenderPass::OpType::LOAD_AND_STORE;
		fogPass.frameBufferResource = m_sceneFrameBufferResource;
		fogPass.material = m_fogMat;
		{
			RenderGraphPassDesc accesses;
			accesses.readColor(m_gBufferFrameBufferResource)
				.readDepth(m_gBufferDepthResource)
				.readColor(m_gBufferNormalResource)
				.writeColor(m_sceneColorResource);
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
			m_bloom.executeBrightPass(graphContext, graphTexture(m_sceneColorResource, &graphContext));
		}).withOutput(m_bloomBrightOutputResource)
			.withOutput("color", m_bloomBrightOutputResource)
			.dependsOn(sceneChainNode);

		RenderGraphPassDesc bloomPass;
		bloomPass.name = "Bloom";
		bloomPass.writeColor(m_sceneColorResource, RenderGraphResourceLayout::ShaderRead)
			.readWriteStorageImage(m_bloomBrightOutputResource, RenderGraphResourceLayout::ShaderRead);
		bloomPass.execute = [this](RenderGraphPassContext& graphContext)
		{
			executeBloomPass(graphContext);
		};
		auto bloomNode = m_renderGraph.addExternalNode(bloomPass)
			.withOutput(m_sceneColorResource)
			.withOutput("sceneColor", m_sceneColorResource)
			.dependsOn(brightNode);
		sceneChainNode = bloomNode;
	}

	RenderGraphNode outputNode;
	if(settings.aaEnabled())
	{
		auto tsaaTargetResource = m_tsaaFrameBufferResources[m_tsaa.targetBufferIndex()];
		auto tsaaHistoryResource = m_tsaaFrameBufferResources[m_tsaa.historyBufferIndex()];
		RenderGraphRasterPassDesc tsaaPass;
		tsaaPass.name = "TSAA";
		tsaaPass.frameBufferResource = tsaaTargetResource;
		tsaaPass.material = m_tsaa.material();
		{
			RenderGraphPassDesc accesses;
			accesses.readColor(m_sceneColorResource)
				.readColor(tsaaHistoryResource)
				.readDepth(m_gBufferDepthResource)
				.writeColor(tsaaTargetResource, RenderGraphResourceLayout::ShaderRead);
			tsaaPass.resourceAccesses = accesses.resourceAccesses;
		}
		outputNode = m_renderGraph.addFullscreenNode(tsaaPass, [this](RenderGraphPassContext& graphContext)
		{
			executeTSAAPass(graphContext);
		}).withOutput(tsaaTargetResource)
			.withOutput("color", tsaaTargetResource)
			.withOutput("history", tsaaHistoryResource)
			.dependsOn(sceneChainNode);
	}
	else
	{
		RenderGraphPassDesc sceneOutputPass;
		sceneOutputPass.name = "SceneOutput";
		sceneOutputPass.readColor(m_sceneColorResource);
		sceneOutputPass.execute = [this](RenderGraphPassContext& graphContext)
		{
			m_outputTexture = graphTexture(m_sceneColorResource, &graphContext);
		};
		outputNode = m_renderGraph.addExternalNode(sceneOutputPass)
			.withOutput(m_sceneColorResource)
			.withOutput("sceneColor", m_sceneColorResource)
			.dependsOn(sceneChainNode);
	}

	auto finalOutputNode = outputNode;
	auto finalSceneResource = outputNode.output();
	if(m_outlinePass.hasOutlineCommands(renderQueue()) && m_outlineOutputResource.isValid())
	{
		RenderGraphPassDesc outlinePass;
		outlinePass.name = "Outline";
		if(finalSceneResource.isValid())
		{
			outlinePass.readColor(finalSceneResource);
		}
		outlinePass.readDepth(m_gBufferDepthResource)
			.writeColor(m_outlineOutputResource, RenderGraphResourceLayout::ShaderRead);
		outlinePass.execute = [this](RenderGraphPassContext& graphContext)
		{
			executeOutlinePass(graphContext);
		};
		finalOutputNode = m_renderGraph.addExternalNode(outlinePass)
			.withOutput(m_outlineOutputResource)
			.withOutput("sceneColor", m_outlineOutputResource)
			.dependsOn(outputNode);
		finalSceneResource = finalOutputNode.output();
	}

	int screenIndex = imageIndex % 2;
	if(screenIndex < 0)
	{
		screenIndex = 0;
	}
	auto screenFrameBufferResource = m_screenFrameBufferResources[screenIndex];
	RenderGraphRasterPassDesc textureToScreenPass;
	textureToScreenPass.name = "Texture To Screen Pass";
	textureToScreenPass.attachments = {
		{ImageFormat::Surface_Format, false},
		{ImageFormat::D24_S8, true},
	};
	textureToScreenPass.opType = DeviceRenderPass::OpType::LOADCLEAR_AND_STORE;
	textureToScreenPass.isOutputToScreen = true;
	textureToScreenPass.frameBufferResource = screenFrameBufferResource;
	textureToScreenPass.material = m_textureToScreenMat;
	{
		RenderGraphPassDesc accesses;
		if(finalSceneResource.isValid())
		{
			accesses.readColor(finalSceneResource);
		}
		accesses.writeColor(screenFrameBufferResource);
		textureToScreenPass.resourceAccesses = accesses.resourceAccesses;
	}
	auto textureToScreenNode = m_renderGraph.addFullscreenNode(textureToScreenPass, [this](RenderGraphPassContext& graphContext)
	{
		executeTextureToScreenPass(graphContext);
	}).dependsOn(finalOutputNode);
	std::string compileMessage;
	if(!m_renderGraph.compile(textureToScreenNode, &compileMessage))
	{
		tlogError("SceneView RenderGraph compile failed:\n%s", compileMessage.c_str());
		return false;
	}
	return true;
}

DeviceFrameBuffer* SceneView::graphFrameBuffer(RenderGraphResourceHandle handle, const RenderGraphPassContext* graphContext) const
{
	if(graphContext)
	{
		auto frameBuffer = graphContext->frameBuffer(handle);
		if(frameBuffer)
		{
			return frameBuffer;
		}
	}
	return m_renderGraph.frameBuffer(handle);
}

DeviceTexture* SceneView::graphTexture(RenderGraphResourceHandle handle, const RenderGraphPassContext* graphContext) const
{
	if(graphContext)
	{
		auto texture = graphContext->texture(handle);
		if(texture)
		{
			return texture;
		}
	}
	return m_renderGraph.texture(handle);
}

DeviceTexture* SceneView::graphDepthTexture(RenderGraphResourceHandle handle, const RenderGraphPassContext* graphContext) const
{
	if(graphContext)
	{
		auto texture = graphContext->depthTexture(handle);
		if(texture)
		{
			return texture;
		}
	}
	return m_renderGraph.depthTexture(handle);
}

size_t SceneView::bindGBufferTextures(DeviceDescriptor* descriptor, int firstBinding, const RenderGraphPassContext* graphContext) const
{
	auto gBufferFrameBuffer = graphFrameBuffer(m_gBufferFrameBufferResource, graphContext);
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
	setCamera(currScene ? currScene->defaultCamera() : nullptr);
	SceneCuller::shared()->collect(this);
	applyCameraToCommands(camera());
}

void SceneView::draw(DeviceRenderCommand* cmd, RenderPath* renderPath)
{
	draw(cmd, renderPath, 0);
}

void SceneView::draw(DeviceRenderCommand* cmd, RenderPath* renderPath, int imageIndex)
{
	if(!buildRenderGraph(imageIndex))
	{
		return;
	}
	RenderGraphContext graphContext(cmd, renderPath, renderQueue());
	m_renderGraph.execute(graphContext);
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

	bindGBufferTextures(descriptorSet, 1, &graphContext);
	descriptorSet->updateDescriptorByBinding(8, m_shadowTextures);
	graphContext.bindSinglePipelineDescriptor();
	graphContext.drawScreenQuad();
}

void SceneView::executePointLightingPass(RenderGraphPassContext& graphContext)
{
	auto backEnd = static_cast<VKRenderBackEnd *>(Engine::shared()->getRenderBackEnd());
	bindGBufferTextures(graphContext.materialDescriptor(), 1, &graphContext);

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
		auto itemDescriptorSet = graphContext.itemDescriptor();
		if(!itemDescriptorSet)
		{
			return;
		}
		DeviceItemBuffer itemBuf = backEnd->getItemBufferPool()->giveMeItemBuffer(sizeof(PointLightUniform));
		PointLightUniform uniform;
		itemBuf.map();
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
		itemBuf.copyFrom(&uniform, sizeof(PointLightUniform));
		itemBuf.unMap();
		itemDescriptorSet->updateDescriptorByBinding(0, &itemBuf);
		graphContext.bindSinglePipelineDescriptor(itemDescriptorSet);
		graphContext.drawSphere();
	}
}

void SceneView::executeSkyPass(RenderGraphPassContext& graphContext)
{
	auto backEnd = static_cast<VKRenderBackEnd *>(Engine::shared()->getRenderBackEnd());
	auto descriptorSet = graphContext.materialDescriptor();
	auto itemDescriptorSet = graphContext.itemDescriptor();
	auto gBufferDepth = graphDepthTexture(m_gBufferDepthResource, &graphContext);
	if(!descriptorSet || !itemDescriptorSet || !gBufferDepth)
	{
		return;
	}

	DeviceItemBuffer itemBuf = backEnd->getItemBufferPool()->giveMeItemBuffer(sizeof(Matrix44));
	itemBuf.map();
	Matrix44 scale;
	scale.setScale(vec3(6360000.0f, 6360000.0f, 6360000.0f));
	Matrix44 m = camera()->getViewProjectionMatrix() * scale;
	itemBuf.copyFrom(&m, sizeof(Matrix44));
	itemBuf.unMap();
	itemDescriptorSet->updateDescriptorByBinding(0, &itemBuf);
	descriptorSet->updateDescriptorByBinding(1, gBufferDepth);
	graphContext.bindSinglePipelineDescriptor(itemDescriptorSet);
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
	auto backEnd = static_cast<VKRenderBackEnd *>(Engine::shared()->getRenderBackEnd());
	auto pipeline = graphContext.pipeline();
	auto descriptorSet = graphContext.materialDescriptor();
	auto itemDescriptorSet = graphContext.itemDescriptor();
	if(!pipeline || !descriptorSet || !itemDescriptorSet)
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
	bindGBufferTextures(descriptorSet, 1, &graphContext);
	DeviceItemBuffer itemBuf = backEnd->getItemBufferPool()->giveMeItemBuffer(sizeof(Matrix44));
	itemBuf.map();
	Matrix44 m = camera()->getViewProjectionMatrix();
	itemBuf.copyFrom(&m, sizeof(Matrix44));
	itemBuf.unMap();
	itemDescriptorSet->updateDescriptorByBinding(0, &itemBuf);
	graphContext.bindSinglePipelineDescriptor(itemDescriptorSet);
	graphContext.drawScreenQuad();
}

void SceneView::executeSSRPass(RenderGraphPassContext& graphContext)
{
	auto backEnd = static_cast<VKRenderBackEnd *>(Engine::shared()->getRenderBackEnd());
	auto sceneColorCopy = graphTexture(m_sceneColorCopyResource, &graphContext);
	auto hbaoOutput = graphTexture(m_hbaoOutputResource, &graphContext);
	auto descriptorSet = graphContext.materialDescriptor();
	auto itemDescriptorSet = graphContext.itemDescriptor();
	if(!sceneColorCopy || !hbaoOutput || !descriptorSet || !itemDescriptorSet)
	{
		return;
	}

	auto gbufferTextureCount = bindGBufferTextures(descriptorSet, 1, &graphContext);
	descriptorSet->updateDescriptorByBinding(static_cast<int>(gbufferTextureCount) + 1, sceneColorCopy);
	descriptorSet->updateDescriptorByBinding(static_cast<int>(gbufferTextureCount) + 2, hbaoOutput);
	DeviceItemBuffer itemBuf = backEnd->getItemBufferPool()->giveMeItemBuffer(sizeof(Matrix44));
	itemBuf.map();
	Matrix44 m = camera()->getViewProjectionMatrix();
	itemBuf.copyFrom(&m, sizeof(Matrix44));
	itemBuf.unMap();
	itemDescriptorSet->updateDescriptorByBinding(0, &itemBuf);
	graphContext.bindSinglePipelineDescriptor(itemDescriptorSet);
	graphContext.drawScreenQuad();
}

void SceneView::executeSSGIPass(RenderGraphPassContext& graphContext)
{
	auto sceneColorCopy = graphTexture(m_sceneColorCopyResource, &graphContext);
	auto gBufferDepth = graphDepthTexture(m_gBufferDepthResource, &graphContext);
	auto gBufferNormal = graphTexture(m_gBufferNormalResource, &graphContext);
	auto gBufferBaseColor = graphTexture(m_gBufferBaseColorResource, &graphContext);
	auto sceneFrameBuffer = graphFrameBuffer(m_sceneFrameBufferResource, &graphContext);
	if(!sceneColorCopy || !gBufferDepth || !gBufferNormal || !gBufferBaseColor || !sceneFrameBuffer)
	{
		return;
	}
	graphContext.renderPath()->addRenderStage(m_ssgi.draw(graphContext.cmd(), sceneColorCopy,
		gBufferDepth,
		gBufferNormal,
		gBufferBaseColor,
		sceneFrameBuffer));
}

void SceneView::executeFogPass(RenderGraphPassContext& graphContext)
{
	auto descriptorSet = graphContext.materialDescriptor();
	if(!descriptorSet)
	{
		return;
	}

	bindGBufferTextures(descriptorSet, 1, &graphContext);
	graphContext.bindSinglePipelineDescriptor();
	graphContext.drawScreenQuad();
}

void SceneView::executeBloomPass(RenderGraphPassContext& graphContext)
{
	m_bloom.drawAfterBright(graphContext.cmd(), graphContext.renderPath());
}

void SceneView::executeTSAAPass(RenderGraphPassContext& graphContext)
{
	auto targetResource = m_tsaaFrameBufferResources[m_tsaa.targetBufferIndex()];
	auto historyResource = m_tsaaFrameBufferResources[m_tsaa.historyBufferIndex()];
	auto historyFrame = graphTexture(historyResource, &graphContext);
	auto targetFrame = graphTexture(targetResource, &graphContext);
	auto sceneColor = graphTexture(m_sceneColorResource, &graphContext);
	auto gBufferDepth = graphDepthTexture(m_gBufferDepthResource, &graphContext);
	if(!historyFrame || !targetFrame || !sceneColor || !gBufferDepth)
	{
		return;
	}
	m_tsaa.executeResolve(graphContext, historyFrame, sceneColor, gBufferDepth);
	m_outputTexture = targetFrame;
}

void SceneView::executeOutlinePass(RenderGraphPassContext& graphContext)
{
	auto gBufferDepth = graphDepthTexture(m_gBufferDepthResource, &graphContext);
	if(!m_outputTexture || !gBufferDepth)
	{
		return;
	}
	m_outputTexture = m_outlinePass.draw(graphContext.cmd(), graphContext.renderPath(), graphContext.sceneQueue(), m_outputTexture, gBufferDepth);
}

void SceneView::executeTextureToScreenPass(RenderGraphPassContext& graphContext)
{
	auto descriptorSet = graphContext.materialDescriptor();
	if(!descriptorSet || !m_outputTexture)
	{
		return;
	}
	descriptorSet->updateDescriptorByBinding(1, m_outputTexture);
	graphContext.bindSinglePipelineDescriptor();
	graphContext.drawScreenQuad();
}

void SceneView::preTick(const RenderSettings& settings)
{
	if(settings.aaEnabled())
	{
		m_tsaa.preTick();
	}
	else if(g_GetCurrScene() && g_GetCurrScene()->defaultCamera())
	{
		g_GetCurrScene()->defaultCamera()->setOffsetPixel(0, 0);
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

void SceneView::setShadowTextures(const std::vector<DeviceTexture*>& shadowTextures)
{
	m_shadowTextures = shadowTextures;
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
