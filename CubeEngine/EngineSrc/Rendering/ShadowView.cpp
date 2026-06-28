#include "ShadowView.h"

#include "3D/ShadowMap/ShadowMap.h"
#include "BackEnd/DeviceFrameBuffer.h"
#include "BackEnd/DeviceRenderPass.h"
#include "BackEnd/DeviceRenderStage.h"
#include "BackEnd/VkRenderBackEnd.h"
#include "Engine/Engine.h"
#include "Rendering/ImageFormat.h"
#include "RenderPath.h"
#include "Scene/SceneCuller.h"
#include "Technique/MaterialInstance.h"

namespace tzw
{
ShadowView::ShadowView(int cascadeIndex)
	: RenderView(RenderViewType::Shadow, cascadeIndex)
	, m_cascadeIndex(cascadeIndex)
	, m_shadowStage(nullptr)
	, m_shadowMat(nullptr)
	, m_shadowInstancedMat(nullptr)
{
}

void ShadowView::init()
{
	auto backEnd = static_cast<VKRenderBackEnd *>(Engine::shared()->getRenderBackEnd());

	m_shadowMat = new MaterialInstance();
	m_shadowMat->loadFromMaterial("Shadow");
	m_shadowInstancedMat = new MaterialInstance();
	m_shadowInstancedMat->loadFromMaterial("ShadowInstance");

	auto shadowRenderPass = backEnd->createDeviceRenderpass_imp();
	shadowRenderPass->init({{ImageFormat::D24_S8, true}}, DeviceRenderPass::OpType::LOADCLEAR_AND_STORE, true);
	auto shadowBuffer = backEnd->createFrameBuffer_imp();
	shadowBuffer->init(ShadowMap::shared()->getShadowMapSize(), ShadowMap::shared()->getShadowMapSize(), shadowRenderPass);
	m_shadowStage = backEnd->createRenderStage_imp();
	m_shadowStage->setName("Shadow Pass");
	m_shadowStage->init(shadowRenderPass, shadowBuffer, static_cast<uint32_t>(RenderFlag::RenderStage::SHADOW));

	addPass(m_shadowStage, static_cast<uint32_t>(RenderFlag::RenderStage::SHADOW), true);
}

void ShadowView::collect()
{
	SceneCuller::shared()->collect(this);
	applyMatricesToCommands(
		ShadowMap::shared()->getLightViewMatrix(),
		ShadowMap::shared()->getLightProjectionMatrix(m_cascadeIndex));
}

void ShadowView::draw(DeviceRenderCommand* cmd, RenderPath* renderPath)
{
	m_shadowStage->prepare(cmd);
	m_shadowStage->beginRenderPass();
	for(auto & command : renderQueue()->getList())
	{
		if(command.batchType() != RenderCommand::RenderBatchType::Single)
		{
			command.setMat(m_shadowInstancedMat);
		}
		else
		{
			command.setMat(m_shadowMat);
		}
		command.m_transInfo.m_viewMatrix = ShadowMap::shared()->getLightViewMatrix();
		command.m_transInfo.m_projectMatrix = ShadowMap::shared()->getLightProjectionMatrix(m_cascadeIndex);
	}
	m_shadowStage->draw(renderQueue(), MaterialTechniqueType::Default);
	m_shadowStage->endRenderPass();
	m_shadowStage->finish();
	renderPath->addRenderStage(m_shadowStage);
}

DeviceTexture* ShadowView::depthTexture() const
{
	return m_shadowStage->getFrameBuffer()->getDepthMap();
}

int ShadowView::cascadeIndex() const
{
	return m_cascadeIndex;
}
}
