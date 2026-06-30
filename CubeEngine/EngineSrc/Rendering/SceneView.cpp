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
}

void SceneView::buildRenderGraph()
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
	m_renderGraph.addRasterPass(gBufferPass, [](RenderGraphPassContext& graphContext)
	{
		graphContext.drawSceneQueue();
	});

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
	m_renderGraph.addFullscreenPass(deferredLightingPass, [this](RenderGraphPassContext& graphContext)
	{
		executeDeferredLightingPass(graphContext);
	});

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
	m_renderGraph.addRasterPass(pointLightingPass, [this](RenderGraphPassContext& graphContext)
	{
		executePointLightingPass(graphContext);
	});

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
	m_renderGraph.addRasterPass(transparentPass, [](RenderGraphPassContext& graphContext)
	{
		graphContext.drawSceneQueue();
	});

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
	m_renderGraph.addRasterPass(skyPass, [this](RenderGraphPassContext& graphContext)
	{
		executeSkyPass(graphContext);
	});

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
	m_renderGraph.addRasterPass(debugWireframePass, [this](RenderGraphPassContext& graphContext)
	{
		executeDebugWireframePass(graphContext);
	});

	const bool needsSceneColorCopy = settings.ssrEnabled() || settings.ssgiEnabled();
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
		m_renderGraph.addFullscreenPass(hbaoPass, [this](RenderGraphPassContext& graphContext)
		{
			executeHBAOPass(graphContext);
		});
	}

	if(needsSceneColorCopy)
	{
		m_renderGraph.addBlitPass("SceneColorCopy", m_sceneColorResource, m_sceneColorCopyResource);
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
		m_renderGraph.addFullscreenPass(ssrPass, [this](RenderGraphPassContext& graphContext)
		{
			executeSSRPass(graphContext);
		});
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
		m_renderGraph.addExternalPass(ssgiPass);
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
		m_renderGraph.addFullscreenPass(fogPass, [this](RenderGraphPassContext& graphContext)
		{
			executeFogPass(graphContext);
		});
	}

	if(settings.bloomEnabled())
	{
		RenderGraphPassDesc bloomPass;
		bloomPass.name = "Bloom";
		bloomPass.readWriteColor(m_sceneColorResource, RenderGraphResourceLayout::ShaderRead);
		bloomPass.execute = [this](RenderGraphPassContext& graphContext)
		{
			executeBloomPass(graphContext);
		};
		m_renderGraph.addExternalPass(bloomPass);
	}

	if(settings.aaEnabled())
	{
		RenderGraphPassDesc tsaaPass;
		tsaaPass.name = "TSAA";
		tsaaPass.readColor(m_sceneColorResource)
			.readDepth(m_gBufferDepthResource);
		tsaaPass.execute = [this](RenderGraphPassContext& graphContext)
		{
			executeTSAAPass(graphContext);
		};
		m_renderGraph.addExternalPass(tsaaPass);
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
		m_renderGraph.addExternalPass(sceneOutputPass);
	}

	RenderGraphPassDesc outlinePass;
	outlinePass.name = "Outline";
	outlinePass.readColor(m_sceneColorResource)
		.readDepth(m_gBufferDepthResource);
	outlinePass.execute = [this](RenderGraphPassContext& graphContext)
	{
		executeOutlinePass(graphContext);
	};
	m_renderGraph.addExternalPass(outlinePass);
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
	buildRenderGraph();
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
	auto sceneColor = graphTexture(m_sceneColorResource, &graphContext);
	if(!sceneColor)
	{
		return;
	}
	m_bloom.draw(graphContext.cmd(), graphContext.renderPath(), sceneColor);
}

void SceneView::executeTSAAPass(RenderGraphPassContext& graphContext)
{
	auto sceneColor = graphTexture(m_sceneColorResource, &graphContext);
	auto gBufferDepth = graphDepthTexture(m_gBufferDepthResource, &graphContext);
	if(!sceneColor || !gBufferDepth)
	{
		return;
	}
	graphContext.renderPath()->addRenderStage(m_tsaa.draw(graphContext.cmd(), sceneColor,  gBufferDepth));
	m_outputTexture = m_tsaa.getOutput()->getTextureList()[0];
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
