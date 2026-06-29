#include "SceneView.h"

#include <cmath>
#include <cstdlib>
#include <random>

#include "3D/ShadowMap/ShadowMap.h"
#include "BackEnd/DeviceFrameBuffer.h"
#include "BackEnd/DeviceMaterial.h"
#include "BackEnd/DeviceRenderCommand.h"
#include "BackEnd/DeviceRenderPass.h"
#include "BackEnd/DeviceRenderStage.h"
#include "BackEnd/VkRenderBackEnd.h"
#include "BackEnd/vk/DevicePipelineVK.h"
#include "BackEnd/vk/DeviceRenderStageVK.h"
#include "BackEnd/vk/DeviceShaderCollectionVK.h"
#include "BackEnd/vk/DeviceTextureVK.h"
#include "Engine/DebugSystem.h"
#include "Engine/Engine.h"
#include "Interface/Drawable3D.h"
#include "Lighting/PointLight.h"
#include "RenderPath.h"
#include "Scene/OctreeScene.h"
#include "Scene/Scene.h"
#include "Scene/SceneCuller.h"
#include "Scene/SceneMgr.h"
#include "Technique/MaterialPool.h"
#include "Texture/Texture.h"
#include "Utility/file/Tfile.h"

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

RenderGraphResourceDesc makeGraphResourceDesc(const char* name, ImageFormat format, TextureRoleEnum role, TextureUsageEnum usage, vec2 size)
{
	RenderGraphResourceDesc desc;
	desc.name = name;
	desc.format = format;
	desc.role = role;
	desc.usage = usage;
	desc.size = size;
	desc.imported = true;
	return desc;
}
}

SceneView::SceneView()
	: RenderView(RenderViewType::Scene)
	, m_gPassStage(nullptr)
	, m_DeferredLightingStage(nullptr)
	, m_PointLightingStage(nullptr)
	, m_skyStage(nullptr)
	, m_debugWireframeStage(nullptr)
	, m_SSRStage(nullptr)
	, m_HBAOStage(nullptr)
	, m_fogStage(nullptr)
	, m_transparentStage(nullptr)
	, m_aaStage(nullptr)
	, m_computeTest(nullptr)
	, m_sceneCopyTex(nullptr)
	, m_outputTexture(nullptr)
	, m_isAAEnable(false)
{
}

void SceneView::init()
{
	auto backEnd = static_cast<VKRenderBackEnd *>(Engine::shared()->getRenderBackEnd());
	auto size = Engine::shared()->winSize();
	m_renderGraph.clearResources();

	auto gBufferRenderPass = backEnd->createDeviceRenderpass_imp();
	gBufferRenderPass->init({
		{ImageFormat::R8G8B8A8, false},
		{ImageFormat::R8G8B8A8, false},
		{ImageFormat::R8G8B8A8_S, false},
		{ImageFormat::R8G8B8A8, false},
		{ImageFormat::D24_S8, true}
		}
		, DeviceRenderPass::OpType::LOADCLEAR_AND_STORE, true);
	m_gBufferFrameBufferResource = m_renderGraph.createFrameBuffer(
		makeGraphResourceDesc("GBufferFrameBuffer", ImageFormat::R8G8B8A8, TextureRoleEnum::AS_COLOR, TextureUsageEnum::SAMPLE_AND_ATTACHMENT, size),
		gBufferRenderPass);
	auto gBuffer = m_renderGraph.frameBuffer(m_gBufferFrameBufferResource);

	m_gPassStage = backEnd->createRenderStage_imp();
	m_gPassStage->setName("GBufferPass");
	m_gPassStage->init(gBufferRenderPass, gBuffer, DrawPassType::GBuffer);
	addPass(m_gPassStage, DrawPassType::GBuffer, true);
	addSubmitDrawPass(DrawPassType::AfterDepthClear);

	auto deferredLightingPass = backEnd->createDeviceRenderpass_imp();
	deferredLightingPass->init({{
		ImageFormat::R16G16B16A16, false}, {ImageFormat::D24_S8, true}}, DeviceRenderPass::OpType::LOADCLEAR_AND_STORE, false);
	m_sceneFrameBufferResource = m_renderGraph.createFrameBuffer(
		makeGraphResourceDesc("SceneFrameBuffer", ImageFormat::R16G16B16A16, TextureRoleEnum::AS_COLOR, TextureUsageEnum::SAMPLE_AND_ATTACHMENT, size),
		deferredLightingPass);
	auto deferredLightingBuffer = m_renderGraph.frameBuffer(m_sceneFrameBufferResource);

	m_DeferredLightingStage = backEnd->createRenderStage_imp();
	m_DeferredLightingStage->init(deferredLightingPass, deferredLightingBuffer);
	m_DeferredLightingStage->setName("Deferred Sun Lighting Stage");
	MaterialInstance* mat = new MaterialInstance();
	mat->loadFromMaterial("DirectLight");
	m_DeferredLightingStage->createSinglePipeline(mat);
	addPass(m_DeferredLightingStage, 0, false);

	MaterialInstance * pointLightMat = new MaterialInstance();
	pointLightMat->loadFromMaterial("PointLight");
	auto pointLightPass = backEnd->createDeviceRenderpass_imp();
	pointLightPass->init({
		{ImageFormat::R16G16B16A16, false}, {ImageFormat::D24_S8, true}}, DeviceRenderPass::OpType::LOAD_AND_STORE, false);
	m_PointLightingStage = backEnd->createRenderStage_imp();
	m_PointLightingStage->init(pointLightPass, m_DeferredLightingStage->getFrameBuffer());
	m_PointLightingStage->createSinglePipeline(pointLightMat);
	m_PointLightingStage->setName("Deferred Point Light Stage");
	addPass(m_PointLightingStage, 0, false);

	auto skyPass = backEnd->createDeviceRenderpass_imp();
	skyPass->init({{
		ImageFormat::R16G16B16A16, false}, {ImageFormat::D24_S8, true}}, DeviceRenderPass::OpType::LOAD_AND_STORE, false);
	m_skyStage = backEnd->createRenderStage_imp();
	m_skyStage->init(skyPass, m_DeferredLightingStage->getFrameBuffer());
	m_skyStage->setName("Sky Stage");

	MaterialInstance * matSkyPass = new MaterialInstance();
	matSkyPass->loadFromMaterial("Sky");
	m_skyStage->createSinglePipeline(matSkyPass);
	addPass(m_skyStage, 0, false);

	MaterialInstance * matHBAO = new MaterialInstance();
	matHBAO->loadFromMaterial("HBAO");
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
	matHBAO->setTex("jitterTex", jitterTex);

	MaterialPool::shared()->addMaterial("HBAO", matHBAO);
	auto HBAOPass = backEnd->createDeviceRenderpass_imp();
	HBAOPass->init({{
		ImageFormat::R16G16B16A16, false}, {ImageFormat::D24_S8, true}}, DeviceRenderPass::OpType::LOADCLEAR_AND_STORE, true);

	m_ssgi.init();
	m_tsaa.init();
	m_outlinePass.init();

	m_HBAOStage = backEnd->createRenderStage_imp();
	m_hbaoFrameBufferResource = m_renderGraph.createFrameBuffer(
		makeGraphResourceDesc("HBAOFrameBuffer", ImageFormat::R16G16B16A16, TextureRoleEnum::AS_COLOR, TextureUsageEnum::SAMPLE_AND_ATTACHMENT, size),
		HBAOPass);
	auto hbaoBuffer = m_renderGraph.frameBuffer(m_hbaoFrameBufferResource);
	m_HBAOStage->init(HBAOPass, hbaoBuffer);
	m_HBAOStage->setName("HBAO Stage");
	m_HBAOStage->createSinglePipeline(matHBAO);
	addPass(m_HBAOStage, 0, false);

	m_sceneCopyTex = new DeviceTextureVK();
	m_sceneCopyTex->initEmpty(size.x, size.y, ImageFormat::R16G16B16A16_SFLOAT,TextureRoleEnum::AS_COLOR, TextureUsageEnum::SAMPLE_AND_ATTACHMENT, 1, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	MaterialInstance * matSSR = new MaterialInstance();
	matSSR->loadFromMaterial("SSR");
	MaterialPool::shared()->addMaterial("SSR", matSSR);
	auto SSRPass = backEnd->createDeviceRenderpass_imp();
	SSRPass->init({{
		ImageFormat::R16G16B16A16, false}, {ImageFormat::D24_S8, true}}, DeviceRenderPass::OpType::LOAD_AND_STORE, false);

	m_SSRStage = backEnd->createRenderStage_imp();
	m_SSRStage->init(SSRPass, m_DeferredLightingStage->getFrameBuffer());
	m_SSRStage->setName("SSR Stage");
	m_SSRStage->createSinglePipeline(matSSR);
	addPass(m_SSRStage, 0, false);

	MaterialInstance * matFog = new MaterialInstance();
	matFog->loadFromMaterial("GlobalFog");
	MaterialPool::shared()->addMaterial("GlobalFog", matFog);
	auto fogPass = backEnd->createDeviceRenderpass_imp();
	fogPass->init({{
		ImageFormat::R16G16B16A16, false}, {ImageFormat::D24_S8, true}}, DeviceRenderPass::OpType::LOAD_AND_STORE, false);

	m_fogStage = backEnd->createRenderStage_imp();
	m_fogStage->init(fogPass, m_DeferredLightingStage->getFrameBuffer());
	m_fogStage->setName("Fog Stage");
	m_fogStage->createSinglePipeline(matFog);
	addPass(m_fogStage, 0, false);

	m_bloom.init(m_DeferredLightingStage->getFrameBuffer());

	auto FXAAPass = backEnd->createDeviceRenderpass_imp();
	FXAAPass->init({{
		ImageFormat::R16G16B16A16, false}, {ImageFormat::D24_S8, true}}, DeviceRenderPass::OpType::LOADCLEAR_AND_STORE, true);
	MaterialInstance * matFXAA = new MaterialInstance();
	matFXAA->loadFromMaterial("FXAA");
	m_fxaaFrameBufferResource = m_renderGraph.createFrameBuffer(
		makeGraphResourceDesc("FXAAFrameBuffer", ImageFormat::R16G16B16A16, TextureRoleEnum::AS_COLOR, TextureUsageEnum::SAMPLE_AND_ATTACHMENT, size),
		FXAAPass);
	auto fxAABuffer = m_renderGraph.frameBuffer(m_fxaaFrameBufferResource);
	m_aaStage = backEnd->createRenderStage_imp();
	m_aaStage->init(FXAAPass, fxAABuffer);
	m_aaStage->setName("FXAA Stage");
	m_aaStage->createSinglePipeline(matFXAA);
	addPass(m_aaStage, 0, false);

	auto transparentPass = backEnd->createDeviceRenderpass_imp();
	transparentPass->init({{
		ImageFormat::R16G16B16A16, false}, {ImageFormat::D24_S8, true}}, DeviceRenderPass::OpType::LOAD_AND_STORE, false);
	m_transparentStage = backEnd->createRenderStage_imp();
	m_transparentStage->setName("TransparentPass");
	m_transparentStage->init(transparentPass, m_DeferredLightingStage->getFrameBuffer(), DrawPassType::Transparent);
	addPass(m_transparentStage, DrawPassType::Transparent, true);

	auto debugWireframePass = backEnd->createDeviceRenderpass_imp();
	debugWireframePass->init({{
		ImageFormat::R16G16B16A16, false}, {ImageFormat::D24_S8, true}}, DeviceRenderPass::OpType::LOAD_AND_STORE, false);
	m_debugWireframeStage = backEnd->createRenderStage_imp();
	m_debugWireframeStage->init(debugWireframePass, m_DeferredLightingStage->getFrameBuffer(), DrawPassType::DebugLayer);
	m_debugWireframeStage->setName("Debug Wireframe Pass");
	if(!backEnd->isWireframeRasterModeSupported())
	{
		DebugSystem::shared()->setWireframeOverlayEnabled(false);
	}
	addPass(m_debugWireframeStage, DrawPassType::DebugLayer, true);

	m_computeTest = backEnd->createRenderStage_imp();
	m_computeTest->initCompute();
	m_computeTest->setName("Compute Test");
	auto computeShader = new DeviceShaderCollectionVK();
	tzw::Data data = tzw::Tfile::shared()->getData("VulkanShaders/VulkanTestCompute.glsl",false);
	computeShader->addShader((const unsigned char *)data.getBytes(),data.getSize(),DeviceShaderType::ComputeShader,(const unsigned char *)"VulkanTestCompute.glsl");
	computeShader->finish();
	m_computeTest->createSingleComputePipeline(computeShader);

	initRenderGraphResources();
	initRenderGraph();
}

void SceneView::initRenderGraphResources()
{
	auto sceneFrameBuffer = m_DeferredLightingStage->getFrameBuffer();
	auto gBufferFrameBuffer = m_gPassStage->getFrameBuffer();
	auto& gBufferTextures = gBufferFrameBuffer->getTextureList();
	auto size = sceneFrameBuffer->getSize();

	m_sceneColorResource = m_sceneFrameBufferResource;
	m_sceneColorCopyResource = m_renderGraph.importTexture(
		makeGraphResourceDesc("SceneColorCopy", ImageFormat::R16G16B16A16_SFLOAT, TextureRoleEnum::AS_COLOR, TextureUsageEnum::SAMPLE_AND_ATTACHMENT, size),
		m_sceneCopyTex);
	m_gBufferDepthResource = m_gBufferFrameBufferResource;
	m_gBufferBaseColorResource = m_gBufferFrameBufferResource;
	if(gBufferTextures.size() > 2)
	{
		m_gBufferNormalResource = m_renderGraph.importTexture(
			makeGraphResourceDesc("GBufferNormal", ImageFormat::R8G8B8A8_S, TextureRoleEnum::AS_COLOR, TextureUsageEnum::SAMPLE_AND_ATTACHMENT, gBufferFrameBuffer->getSize()),
			gBufferTextures[2]);
	}
	m_hbaoOutputResource = m_hbaoFrameBufferResource;
}

void SceneView::initRenderGraph()
{
	m_renderGraph.clear();

	RenderGraphPassDesc hbaoPass;
	hbaoPass.name = "HBAO";
	hbaoPass.stage = m_HBAOStage;
	hbaoPass.consumedDrawPassMask = DrawPassType::Unset;
	hbaoPass.execute = [this](RenderGraphContext& graphContext)
	{
		executeHBAOPass(graphContext);
	};
	m_renderGraph.addPass(hbaoPass);

	RenderGraphPassDesc sceneColorCopyPass;
	sceneColorCopyPass.name = "SceneColorCopy";
	sceneColorCopyPass.consumedDrawPassMask = DrawPassType::Unset;
	sceneColorCopyPass.execute = [this](RenderGraphContext& graphContext)
	{
		executeSceneColorCopyPass(graphContext);
	};
	m_renderGraph.addPass(sceneColorCopyPass);

	RenderGraphPassDesc ssrPass;
	ssrPass.name = "SSR";
	ssrPass.stage = m_SSRStage;
	ssrPass.consumedDrawPassMask = DrawPassType::Unset;
	ssrPass.execute = [this](RenderGraphContext& graphContext)
	{
		executeSSRPass(graphContext);
	};
	m_renderGraph.addPass(ssrPass);

	RenderGraphPassDesc ssgiPass;
	ssgiPass.name = "SSGI";
	ssgiPass.consumedDrawPassMask = DrawPassType::Unset;
	ssgiPass.execute = [this](RenderGraphContext& graphContext)
	{
		executeSSGIPass(graphContext);
	};
	m_renderGraph.addPass(ssgiPass);

	RenderGraphPassDesc fogPass;
	fogPass.name = "Fog";
	fogPass.stage = m_fogStage;
	fogPass.consumedDrawPassMask = DrawPassType::Unset;
	fogPass.execute = [this](RenderGraphContext& graphContext)
	{
		executeFogPass(graphContext);
	};
	m_renderGraph.addPass(fogPass);

	RenderGraphPassDesc bloomPass;
	bloomPass.name = "Bloom";
	bloomPass.consumedDrawPassMask = DrawPassType::Unset;
	bloomPass.execute = [this](RenderGraphContext& graphContext)
	{
		executeBloomPass(graphContext);
	};
	m_renderGraph.addPass(bloomPass);

	RenderGraphPassDesc tsaaPass;
	tsaaPass.name = "TSAA";
	tsaaPass.consumedDrawPassMask = DrawPassType::Unset;
	tsaaPass.execute = [this](RenderGraphContext& graphContext)
	{
		executeTSAAPass(graphContext);
	};
	m_renderGraph.addPass(tsaaPass);

	RenderGraphPassDesc outlinePass;
	outlinePass.name = "Outline";
	outlinePass.consumedDrawPassMask = DrawPassType::Unset;
	outlinePass.execute = [this](RenderGraphContext& graphContext)
	{
		executeOutlinePass(graphContext);
	};
	m_renderGraph.addPass(outlinePass);
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
	auto backEnd = static_cast<VKRenderBackEnd *>(Engine::shared()->getRenderBackEnd());
	auto renderQueues = renderQueue();

	m_gPassStage->prepare(cmd);
	m_gPassStage->beginRenderPass();
	m_gPassStage->draw(renderQueues, MaterialTechniqueType::Default);
	m_gPassStage->endRenderPass();
	m_gPassStage->finish();
	renderPath->addRenderStage(m_gPassStage);

	{
		m_DeferredLightingStage->prepare(cmd);
		m_DeferredLightingStage->beginRenderPass();
		auto material = m_DeferredLightingStage->getSolorDeviceMaterial();

		Matrix44 lightVPList[SHADOWMAP_CASCADE_NUM] = {};
		float shadowEnd[SHADOWMAP_CASCADE_NUM] = {};
		for(int i = 0; i < SHADOWMAP_CASCADE_NUM; i++)
		{
			shadowEnd[i] =  ShadowMap::shared()->getCascadeEnd(i);
			lightVPList[i] = ShadowMap::shared()->getLightProjectionMatrix(i) * ShadowMap::shared()->getLightViewMatrix();
		}
		material->updateUniformSingle("TU_LightVP",lightVPList, sizeof(lightVPList));
		material->updateUniformSingle("TU_ShadowMapEnd", shadowEnd, sizeof(shadowEnd));

		auto gbufferTex = m_gPassStage->getFrameBuffer()->getTextureList();
		for(int i =0; i < gbufferTex.size(); i++)
		{
			auto tex = gbufferTex[i];
			material->getMaterialDescriptorSet()->updateDescriptorByBinding(i + 1, tex);
		}
		material->getMaterialDescriptorSet()->updateDescriptorByBinding(8, m_shadowTextures);

		m_DeferredLightingStage->bindSinglePipelineDescriptor();
		m_DeferredLightingStage->drawScreenQuad();
		m_DeferredLightingStage->endRenderPass();
		m_DeferredLightingStage->finish();
		renderPath->addRenderStage(m_DeferredLightingStage);
	}

	{
		m_PointLightingStage->prepare(cmd);
		m_PointLightingStage->beginRenderPass();
		auto gbufferTex = m_gPassStage->getFrameBuffer()->getTextureList();
		for(int i =0; i < gbufferTex.size(); i++)
		{
			auto tex = gbufferTex[i];
			m_PointLightingStage->getSolorDeviceMaterial()->getMaterialDescriptorSet()->updateDescriptorByBinding(i + 1, tex);
		}
		std::vector<Drawable3D *> pointlightList;
		auto currScene = g_GetCurrScene();
		currScene->getOctreeScene()->cullingByCameraExtraFlag(camera(), static_cast<uint32_t>(DrawableFlag::PointLight), DrawPassType::All,pointlightList);
		for(auto obj : pointlightList)
		{
			PointLight* p = static_cast<PointLight*>(obj);
			if(!p->getIsVisible())
				continue;;
			DeviceItemBuffer itemBuf = backEnd->getItemBufferPool()->giveMeItemBuffer(sizeof(PointLightUniform));
			PointLightUniform uniform;
			DeviceDescriptor * itemDescriptorSet = static_cast<DevicePipelineVK *>(m_PointLightingStage->getSinglePipeline())->giveItemWiseDescriptorSet();
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
			m_PointLightingStage->bindSinglePipelineDescriptor(itemDescriptorSet);
			m_PointLightingStage->drawSphere();
		}
		m_PointLightingStage->endRenderPass();
		m_PointLightingStage->finish();
		renderPath->addRenderStage(m_PointLightingStage);
	}

	{
		m_transparentStage->prepare(cmd);
		m_transparentStage->beginRenderPass();
		m_transparentStage->draw(renderQueues, MaterialTechniqueType::Default);
		m_transparentStage->endRenderPass();
		m_transparentStage->finish();
		renderPath->addRenderStage(m_transparentStage);
	}

	{
		m_skyStage->prepare(cmd);
		m_skyStage->beginRenderPass();
		DeviceItemBuffer itemBuf = backEnd->getItemBufferPool()->giveMeItemBuffer(sizeof(Matrix44));
		DeviceDescriptor * itemDescriptorSet = static_cast<DevicePipelineVK *>(m_skyStage->getSinglePipeline())->giveItemWiseDescriptorSet();
		itemBuf.map();
		Matrix44 scale;
		scale.setScale(vec3(6360000.0f, 6360000.0f, 6360000.0f));
		Matrix44 m = camera()->getViewProjectionMatrix() * scale;
		itemBuf.copyFrom(&m, sizeof(Matrix44));
		itemBuf.unMap();
		itemDescriptorSet->updateDescriptorByBinding(0, &itemBuf);
		auto tex = m_gPassStage->getFrameBuffer()->getDepthMap();
		m_skyStage->getSolorDeviceMaterial()->getMaterialDescriptorSet()->updateDescriptorByBinding(1, tex);
		m_skyStage->bindSinglePipelineDescriptor(itemDescriptorSet);
		m_skyStage->drawSphere();
		m_skyStage->endRenderPass();
		m_skyStage->finish();
		renderPath->addRenderStage(m_skyStage);
	}

	{
		if (DebugSystem::shared()->isWireframeOverlayEnabled())
		{
			auto debugQueue = DebugSystem::shared()->buildWireframeQueue(renderQueues);
			if (debugQueue && !debugQueue->getList().empty())
			{
				m_debugWireframeStage->prepare(cmd);
				m_debugWireframeStage->beginRenderPass();
				m_debugWireframeStage->draw(debugQueue, MaterialTechniqueType::Default);
				m_debugWireframeStage->endRenderPass();
				m_debugWireframeStage->finish();
				renderPath->addRenderStage(m_debugWireframeStage);
			}
		}
	}

	RenderGraphContext graphContext(cmd, renderPath, renderQueues);
	m_renderGraph.execute(graphContext);
}

void SceneView::executeHBAOPass(RenderGraphContext& graphContext)
{
	auto hbaoStage = graphContext.stage();
	if(!hbaoStage)
	{
		return;
	}

	auto backEnd = static_cast<VKRenderBackEnd *>(Engine::shared()->getRenderBackEnd());
	Matrix44 proj = camera()->projection();
	const float* P = proj.data();
	float R = 0.8;
	hbaoStage->getSinglePipeline()->getMat()->setVar("TU_RadiusInfo", vec4(R, R * R, tanf(camera()->getFov() * 0.5f* 3.14 / 180.0), 0.0));
	vec4 projInfoPerspective = vec4(
		2.0f / (P[4 * 0 + 0]),
		2.0f / (P[4 * 1 + 1]),
		-(1.0f - P[4 * 2 + 0]) / P[4 * 0 + 0],
		-(1.0f + P[4 * 2 + 1]) / P[4 * 1 + 1]
	);
	hbaoStage->getSinglePipeline()->getMat()->setVar("TU_ProjInfo", projInfoPerspective);
	hbaoStage->prepare(graphContext.cmd());
	hbaoStage->beginRenderPass();
	auto gbufferTex = m_gPassStage->getFrameBuffer()->getTextureList();
	for(int i =0; i < gbufferTex.size(); i++)
	{
		auto tex = gbufferTex[i];
		hbaoStage->getSolorDeviceMaterial()->getMaterialDescriptorSet()->updateDescriptorByBinding(i + 1, tex);
	}
	DeviceItemBuffer itemBuf = backEnd->getItemBufferPool()->giveMeItemBuffer(sizeof(Matrix44));
	DeviceDescriptor * itemDescriptorSet = static_cast<DevicePipelineVK *>(hbaoStage->getSinglePipeline())->giveItemWiseDescriptorSet();
	itemBuf.map();
	Matrix44 m = camera()->getViewProjectionMatrix();
	itemBuf.copyFrom(&m, sizeof(Matrix44));
	itemBuf.unMap();
	itemDescriptorSet->updateDescriptorByBinding(0, &itemBuf);
	hbaoStage->bindSinglePipelineDescriptor(itemDescriptorSet);
	hbaoStage->drawScreenQuad();
	hbaoStage->endRenderPass();
	hbaoStage->finish();
}

void SceneView::executeSceneColorCopyPass(RenderGraphContext& graphContext)
{
	auto backEnd = static_cast<VKRenderBackEnd *>(Engine::shared()->getRenderBackEnd());
	auto sceneColor = graphContext.texture(m_sceneColorResource);
	auto sceneColorCopy = graphContext.texture(m_sceneColorCopyResource);
	if(!sceneColor)
	{
		sceneColor = m_DeferredLightingStage->getFrameBuffer()->getTextureList()[0];
	}
	if(!sceneColorCopy)
	{
		sceneColorCopy = m_sceneCopyTex;
	}
	backEnd->blitTexture(static_cast<DeviceRenderCommandVK *>(graphContext.cmd())->getVK(),
		static_cast<DeviceTextureVK *>(sceneColor),
		static_cast<DeviceTextureVK *>(sceneColorCopy),
		m_DeferredLightingStage->getFrameBuffer()->getSize(),
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

void SceneView::executeSSRPass(RenderGraphContext& graphContext)
{
	auto ssrStage = graphContext.stage();
	if(!ssrStage)
	{
		return;
	}

	auto backEnd = static_cast<VKRenderBackEnd *>(Engine::shared()->getRenderBackEnd());
	ssrStage->prepare(graphContext.cmd());
	ssrStage->beginRenderPass();
	auto gbufferTex = m_gPassStage->getFrameBuffer()->getTextureList();
	for(int i =0; i < gbufferTex.size(); i++)
	{
		auto tex = gbufferTex[i];
		ssrStage->getSolorDeviceMaterial()->getMaterialDescriptorSet()->updateDescriptorByBinding(i + 1, tex);
	}
	auto sceneColorCopy = graphContext.texture(m_sceneColorCopyResource);
	auto hbaoOutput = graphContext.texture(m_hbaoOutputResource);
	if(!sceneColorCopy)
	{
		sceneColorCopy = m_sceneCopyTex;
	}
	if(!hbaoOutput)
	{
		hbaoOutput = m_HBAOStage->getFrameBuffer()->getTextureList()[0];
	}
	ssrStage->getSolorDeviceMaterial()->getMaterialDescriptorSet()->updateDescriptorByBinding(gbufferTex.size() + 1, sceneColorCopy);
	ssrStage->getSolorDeviceMaterial()->getMaterialDescriptorSet()->updateDescriptorByBinding(gbufferTex.size() + 2, hbaoOutput);
	DeviceItemBuffer itemBuf = backEnd->getItemBufferPool()->giveMeItemBuffer(sizeof(Matrix44));
	DeviceDescriptor * itemDescriptorSet = static_cast<DevicePipelineVK *>(ssrStage->getSinglePipeline())->giveItemWiseDescriptorSet();
	itemBuf.map();
	Matrix44 m = camera()->getViewProjectionMatrix();
	itemBuf.copyFrom(&m, sizeof(Matrix44));
	itemBuf.unMap();
	itemDescriptorSet->updateDescriptorByBinding(0, &itemBuf);
	ssrStage->bindSinglePipelineDescriptor(itemDescriptorSet);
	ssrStage->drawScreenQuad();
	ssrStage->endRenderPass();
	ssrStage->finish();
}

void SceneView::executeSSGIPass(RenderGraphContext& graphContext)
{
	auto sceneColorCopy = graphContext.texture(m_sceneColorCopyResource);
	auto gBufferDepth = graphContext.depthTexture(m_gBufferDepthResource);
	auto gBufferNormal = graphContext.texture(m_gBufferNormalResource);
	auto gBufferBaseColor = graphContext.texture(m_gBufferBaseColorResource);
	auto sceneFrameBuffer = graphContext.frameBuffer(m_sceneFrameBufferResource);
	if(!sceneColorCopy)
	{
		sceneColorCopy = m_sceneCopyTex;
	}
	if(!gBufferDepth)
	{
		gBufferDepth = m_gPassStage->getFrameBuffer()->getDepthMap();
	}
	if(!gBufferNormal)
	{
		gBufferNormal = m_gPassStage->getFrameBuffer()->getTextureList()[2];
	}
	if(!gBufferBaseColor)
	{
		gBufferBaseColor = m_gPassStage->getFrameBuffer()->getTextureList()[0];
	}
	if(!sceneFrameBuffer)
	{
		sceneFrameBuffer = m_SSRStage->getFrameBuffer();
	}
	graphContext.renderPath()->addRenderStage(m_ssgi.draw(graphContext.cmd(), sceneColorCopy,
		gBufferDepth,
		gBufferNormal,
		gBufferBaseColor,
		sceneFrameBuffer));
}

void SceneView::executeFogPass(RenderGraphContext& graphContext)
{
	auto fogStage = graphContext.stage();
	if(!fogStage)
	{
		return;
	}

	fogStage->prepare(graphContext.cmd());
	fogStage->beginRenderPass();
	auto gbufferTex = m_gPassStage->getFrameBuffer()->getTextureList();
	for(int i =0; i < gbufferTex.size(); i++)
	{
		auto tex = gbufferTex[i];
		fogStage->getSolorDeviceMaterial()->getMaterialDescriptorSet()->updateDescriptorByBinding(i + 1, tex);
	}
	fogStage->bindSinglePipelineDescriptor();
	fogStage->drawScreenQuad();
	fogStage->endRenderPass();
	fogStage->finish();
}

void SceneView::executeBloomPass(RenderGraphContext& graphContext)
{
	auto sceneColor = graphContext.texture(m_sceneColorResource);
	if(!sceneColor)
	{
		sceneColor = m_DeferredLightingStage->getFrameBuffer()->getTextureList()[0];
	}
	m_bloom.draw(graphContext.cmd(), graphContext.renderPath(), sceneColor);
}

void SceneView::executeTSAAPass(RenderGraphContext& graphContext)
{
	if(m_isAAEnable)
	{
		auto gBufferDepth = graphContext.depthTexture(m_gBufferDepthResource);
		if(!gBufferDepth)
		{
			gBufferDepth = m_gPassStage->getFrameBuffer()->getDepthMap();
		}
		graphContext.renderPath()->addRenderStage(m_tsaa.draw(graphContext.cmd(), m_fogStage->getFrameBuffer()->getTextureList()[0],  gBufferDepth));
		m_outputTexture = m_tsaa.getOutput()->getTextureList()[0];
	}
	else
	{
		if(camera())
		{
			camera()->setOffsetPixel(0, 0);
		}
		m_outputTexture = m_fogStage->getFrameBuffer()->getTextureList()[0];
	}
}

void SceneView::executeOutlinePass(RenderGraphContext& graphContext)
{
	auto gBufferDepth = graphContext.depthTexture(m_gBufferDepthResource);
	if(!gBufferDepth)
	{
		gBufferDepth = m_gPassStage->getFrameBuffer()->getDepthMap();
	}
	m_outputTexture = m_outlinePass.draw(graphContext.cmd(), graphContext.renderPath(), graphContext.sceneQueue(), m_outputTexture, gBufferDepth);
}

void SceneView::preTick(bool isAAEnable)
{
	m_isAAEnable = isAAEnable;
	if(m_isAAEnable)
	{
		m_tsaa.preTick();
	}
	else if(g_GetCurrScene() && g_GetCurrScene()->defaultCamera())
	{
		g_GetCurrScene()->defaultCamera()->setOffsetPixel(0, 0);
	}
	m_ssgi.preTick();
}

void SceneView::setAAEnabled(bool isAAEnable)
{
	m_isAAEnable = isAAEnable;
}

void SceneView::setShadowTextures(const std::vector<DeviceTexture*>& shadowTextures)
{
	m_shadowTextures = shadowTextures;
}

DeviceTexture* SceneView::outputTexture() const
{
	return m_outputTexture;
}

DeviceRenderStage* SceneView::gPassStage() const
{
	return m_gPassStage;
}

RenderQueue* SceneView::sceneQueue()
{
	return renderQueue();
}
}
