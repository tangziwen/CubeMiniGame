#include "ShadowView.h"

#include "3D/ShadowMap/ShadowMap.h"
#include "Rendering/ImageFormat.h"
#include "Scene/SceneCuller.h"
#include "Technique/MaterialInstance.h"

namespace tzw
{
ShadowView::ShadowView(RenderGraph& graph, int cascadeIndex)
	: RenderView(graph, RenderViewType::Shadow, cascadeIndex)
	, m_cascadeIndex(cascadeIndex)
	, m_shadowMat(nullptr)
	, m_shadowInstancedMat(nullptr)
{
}

void ShadowView::init()
{
	m_shadowMat = new MaterialInstance();
	m_shadowMat->loadFromMaterial("Shadow");
	m_shadowInstancedMat = new MaterialInstance();
	m_shadowInstancedMat->loadFromMaterial("ShadowInstance");

	RenderGraphResourceDesc desc;
	desc.name = "Shadow." + std::to_string(m_cascadeIndex);
	desc.format = ImageFormat::D24_S8;
	desc.role = TextureRoleEnum::AS_DEPTH;
	desc.size = vec2(ShadowMap::shared()->getShadowMapSize(), ShadowMap::shared()->getShadowMapSize());
	m_frameBuffer = m_renderGraph.createFrameBuffer(desc, {{ImageFormat::D24_S8, true}},
		DeviceRenderPass::OpType::LOADCLEAR_AND_STORE, true);
	m_depth = m_renderGraph.depthAttachment(m_frameBuffer);
	addSubmitDrawPass(DrawPassType::Shadow);
}

void ShadowView::collect()
{
	SceneCuller::shared()->collect(this);
	applyMatricesToCommands(
		ShadowMap::shared()->getLightViewMatrix(),
		ShadowMap::shared()->getLightProjectionMatrix(m_cascadeIndex));
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
	}
}

RenderGraphNode ShadowView::buildRenderGraph()
{
	RenderGraphRasterPassDesc pass;
	pass.name = "CSM Cascade " + std::to_string(m_cascadeIndex);
	pass.frameBufferResource = m_frameBuffer;
	pass.drawPassMask = DrawPassType::Shadow;
	pass.consumesSceneQueue = true;
	pass.sceneQueue = renderQueue();
	RenderGraphPassDesc accesses;
	accesses.writeDepth(m_depth, RenderGraphResourceLayout::DepthRead);
	pass.resourceAccesses = accesses.resourceAccesses;
	return m_renderGraph.addRasterNode(pass, [](RenderGraphPassContext& context)
	{
		context.drawSceneQueue();
	}).withOutput("depth", m_depth);
}

int ShadowView::cascadeIndex() const
{
	return m_cascadeIndex;
}
}
