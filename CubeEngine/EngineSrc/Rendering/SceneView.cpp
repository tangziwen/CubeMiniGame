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

	auto gBufferRenderPass = backEnd->createDeviceRenderpass_imp();
	gBufferRenderPass->init({
		{ImageFormat::R8G8B8A8, false},
		{ImageFormat::R8G8B8A8, false},
		{ImageFormat::R8G8B8A8_S, false},
		{ImageFormat::R8G8B8A8, false},
		{ImageFormat::D24_S8, true}
		}
		, DeviceRenderPass::OpType::LOADCLEAR_AND_STORE, true);
	auto gBuffer = backEnd->createFrameBuffer_imp();
	gBuffer->init(size.x, size.y, gBufferRenderPass);

	m_gPassStage = backEnd->createRenderStage_imp();
	m_gPassStage->setName("GBufferPass");
	m_gPassStage->init(gBufferRenderPass, gBuffer, static_cast<uint32_t>(RenderFlag::RenderStage::COMMON));
	addPass(m_gPassStage, static_cast<uint32_t>(RenderFlag::RenderStage::COMMON), true);
	addSubmitStage(static_cast<uint32_t>(RenderFlag::RenderStage::AFTER_DEPTH_CLEAR));

	auto deferredLightingPass = backEnd->createDeviceRenderpass_imp();
	deferredLightingPass->init({{
		ImageFormat::R16G16B16A16, false}, {ImageFormat::D24_S8, true}}, DeviceRenderPass::OpType::LOADCLEAR_AND_STORE, false);
	auto deferredLightingBuffer= backEnd->createFrameBuffer_imp();
	deferredLightingBuffer->init(size.x, size.y, deferredLightingPass);

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
	auto hbaoBuffer= backEnd->createFrameBuffer_imp();
	hbaoBuffer->init(size.x, size.y, HBAOPass);
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
	auto fxAABuffer = backEnd->createFrameBuffer_imp();
	fxAABuffer->init(size.x, size.y, FXAAPass);
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
	m_transparentStage->init(transparentPass, m_DeferredLightingStage->getFrameBuffer(), static_cast<uint32_t>(RenderFlag::RenderStage::TRANSPARENT));
	addPass(m_transparentStage, static_cast<uint32_t>(RenderFlag::RenderStage::TRANSPARENT), true);

	auto debugWireframePass = backEnd->createDeviceRenderpass_imp();
	debugWireframePass->init({{
		ImageFormat::R16G16B16A16, false}, {ImageFormat::D24_S8, true}}, DeviceRenderPass::OpType::LOAD_AND_STORE, false);
	m_debugWireframeStage = backEnd->createRenderStage_imp();
	m_debugWireframeStage->init(debugWireframePass, m_DeferredLightingStage->getFrameBuffer(), static_cast<uint32_t>(RenderFlag::RenderStage::DEBUG_LAYER));
	m_debugWireframeStage->setName("Debug Wireframe Pass");
	if(!backEnd->isWireframeRasterModeSupported())
	{
		DebugSystem::shared()->setWireframeOverlayEnabled(false);
	}
	addPass(m_debugWireframeStage, static_cast<uint32_t>(RenderFlag::RenderStage::DEBUG_LAYER), true);

	m_computeTest = backEnd->createRenderStage_imp();
	m_computeTest->initCompute();
	m_computeTest->setName("Compute Test");
	auto computeShader = new DeviceShaderCollectionVK();
	tzw::Data data = tzw::Tfile::shared()->getData("VulkanShaders/VulkanTestCompute.glsl",false);
	computeShader->addShader((const unsigned char *)data.getBytes(),data.getSize(),DeviceShaderType::ComputeShader,(const unsigned char *)"VulkanTestCompute.glsl");
	computeShader->finish();
	m_computeTest->createSingleComputePipeline(computeShader);
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
		currScene->getOctreeScene()->cullingByCameraExtraFlag(camera(), static_cast<uint32_t>(DrawableFlag::PointLight), static_cast<uint32_t>(RenderFlag::RenderStage::All),pointlightList);
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

	{
		Matrix44 proj = camera()->projection();
		const float* P = proj.data();
		float R = 0.8;
		m_HBAOStage->getSinglePipeline()->getMat()->setVar("TU_RadiusInfo", vec4(R, R * R, tanf(camera()->getFov() * 0.5f* 3.14 / 180.0), 0.0));
		vec4 projInfoPerspective = vec4(
			2.0f / (P[4 * 0 + 0]),
			2.0f / (P[4 * 1 + 1]),
			-(1.0f - P[4 * 2 + 0]) / P[4 * 0 + 0],
			-(1.0f + P[4 * 2 + 1]) / P[4 * 1 + 1]
		);
		m_HBAOStage->getSinglePipeline()->getMat()->setVar("TU_ProjInfo", projInfoPerspective);
		m_HBAOStage->prepare(cmd);
		m_HBAOStage->beginRenderPass();
		auto gbufferTex = m_gPassStage->getFrameBuffer()->getTextureList();
		for(int i =0; i < gbufferTex.size(); i++)
		{
			auto tex = gbufferTex[i];
			m_HBAOStage->getSolorDeviceMaterial()->getMaterialDescriptorSet()->updateDescriptorByBinding(i + 1, tex);
		}
		DeviceItemBuffer itemBuf = backEnd->getItemBufferPool()->giveMeItemBuffer(sizeof(Matrix44));
		DeviceDescriptor * itemDescriptorSet = static_cast<DevicePipelineVK *>(m_HBAOStage->getSinglePipeline())->giveItemWiseDescriptorSet();
		itemBuf.map();
		Matrix44 m = camera()->getViewProjectionMatrix();
		itemBuf.copyFrom(&m, sizeof(Matrix44));
		itemBuf.unMap();
		itemDescriptorSet->updateDescriptorByBinding(0, &itemBuf);
		m_HBAOStage->bindSinglePipelineDescriptor(itemDescriptorSet);
		m_HBAOStage->drawScreenQuad();
		m_HBAOStage->endRenderPass();
		m_HBAOStage->finish();
		renderPath->addRenderStage(m_HBAOStage);
	}

	backEnd->blitTexture(static_cast<DeviceRenderCommandVK *>(cmd)->getVK(),
		static_cast<DeviceTextureVK *>(m_DeferredLightingStage->getFrameBuffer()->getTextureList()[0]),
		static_cast<DeviceTextureVK *>(m_sceneCopyTex),
		m_DeferredLightingStage->getFrameBuffer()->getSize(),
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	{
		m_SSRStage->prepare(cmd);
		m_SSRStage->beginRenderPass();
		auto gbufferTex = m_gPassStage->getFrameBuffer()->getTextureList();
		for(int i =0; i < gbufferTex.size(); i++)
		{
			auto tex = gbufferTex[i];
			m_SSRStage->getSolorDeviceMaterial()->getMaterialDescriptorSet()->updateDescriptorByBinding(i + 1, tex);
		}
		m_SSRStage->getSolorDeviceMaterial()->getMaterialDescriptorSet()->updateDescriptorByBinding(gbufferTex.size() + 1, m_sceneCopyTex);
		m_SSRStage->getSolorDeviceMaterial()->getMaterialDescriptorSet()->updateDescriptorByBinding(gbufferTex.size() + 2, m_HBAOStage->getFrameBuffer()->getTextureList()[0]);
		DeviceItemBuffer itemBuf = backEnd->getItemBufferPool()->giveMeItemBuffer(sizeof(Matrix44));
		DeviceDescriptor * itemDescriptorSet = static_cast<DevicePipelineVK *>(m_SSRStage->getSinglePipeline())->giveItemWiseDescriptorSet();
		itemBuf.map();
		Matrix44 m = camera()->getViewProjectionMatrix();
		itemBuf.copyFrom(&m, sizeof(Matrix44));
		itemBuf.unMap();
		itemDescriptorSet->updateDescriptorByBinding(0, &itemBuf);
		m_SSRStage->bindSinglePipelineDescriptor(itemDescriptorSet);
		m_SSRStage->drawScreenQuad();
		m_SSRStage->endRenderPass();
		m_SSRStage->finish();
		renderPath->addRenderStage(m_SSRStage);
	}

	renderPath->addRenderStage(m_ssgi.draw(cmd, m_sceneCopyTex,
		m_gPassStage->getFrameBuffer()->getDepthMap(),
		m_gPassStage->getFrameBuffer()->getTextureList()[2],
		m_gPassStage->getFrameBuffer()->getTextureList()[0],
		m_SSRStage->getFrameBuffer()));

	{
		m_fogStage->prepare(cmd);
		m_fogStage->beginRenderPass();
		auto gbufferTex = m_gPassStage->getFrameBuffer()->getTextureList();
		for(int i =0; i < gbufferTex.size(); i++)
		{
			auto tex = gbufferTex[i];
			m_fogStage->getSolorDeviceMaterial()->getMaterialDescriptorSet()->updateDescriptorByBinding(i + 1, tex);
		}
		m_fogStage->bindSinglePipelineDescriptor();
		m_fogStage->drawScreenQuad();
		m_fogStage->endRenderPass();
		m_fogStage->finish();
		renderPath->addRenderStage(m_fogStage);
	}

	m_bloom.draw(cmd, renderPath, m_DeferredLightingStage->getFrameBuffer()->getTextureList()[0]);

	if(m_isAAEnable)
	{
		renderPath->addRenderStage(m_tsaa.draw(cmd, m_fogStage->getFrameBuffer()->getTextureList()[0],  m_gPassStage->getFrameBuffer()->getDepthMap()));
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

	m_outputTexture = m_outlinePass.draw(cmd, renderPath, renderQueues, m_outputTexture, m_gPassStage->getFrameBuffer()->getDepthMap());
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
