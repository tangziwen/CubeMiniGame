#include "OutlinePass.h"

#include "BackEnd/DeviceDescriptor.h"
#include "Interface/Drawable3D.h"
#include "Rendering/RenderCommand.h"
#include "Rendering/RenderGraph.h"
#include "Technique/MaterialInstance.h"

namespace tzw
{
void OutlinePass::init()
{
	if(m_maskMaterial && m_compositeMaterial)
	{
		return;
	}
	m_maskMaterial = new MaterialInstance();
	m_maskMaterial->loadFromMaterial("OutlineMask");

	m_compositeMaterial = new MaterialInstance();
	m_compositeMaterial->loadFromMaterial("OutlineComposite");
	m_compositeMaterial->setVar("TU_outlineWidth", 2.0f);
	m_compositeMaterial->setVar("TU_depthEpsilon", 0.0005f);
}

bool OutlinePass::hasOutlineCommands(const RenderQueue* sourceQueue) const
{
	if(!sourceQueue)
	{
		return false;
	}
	for(const auto& command : sourceQueue->getList())
	{
		if(isOutlineCommand(command))
		{
			return true;
		}
	}
	return false;
}

void OutlinePass::executeMask(RenderGraphPassContext& graphContext)
{
	if(buildOutlineQueue(graphContext.sceneQueue()))
	{
		graphContext.drawQueue(&m_outlineQueue);
	}
}

void OutlinePass::executeComposite(RenderGraphPassContext& graphContext, DeviceTexture* sceneColor,
	DeviceTexture* maskColor, DeviceTexture* maskDepth, DeviceTexture* sceneDepth)
{
	auto descriptor = graphContext.materialDescriptor();
	if(!descriptor || !sceneColor || !maskColor || !maskDepth || !sceneDepth)
	{
		return;
	}
	descriptor->updateDescriptorByBinding(1, sceneColor);
	descriptor->updateDescriptorByBinding(2, maskColor);
	descriptor->updateDescriptorByBinding(3, maskDepth);
	descriptor->updateDescriptorByBinding(4, sceneDepth);
	graphContext.bindSinglePipelineDescriptor();
	graphContext.drawScreenQuad();
}

MaterialInstance* OutlinePass::maskMaterial() const
{
	return m_maskMaterial;
}

MaterialInstance* OutlinePass::compositeMaterial() const
{
	return m_compositeMaterial;
}

bool OutlinePass::isOutlineCommand(const RenderCommand& command) const
{
	auto drawableObj = const_cast<RenderCommand&>(command).getDrawableObj();
	if(command.batchType() != RenderCommand::RenderBatchType::Single || !drawableObj)
	{
		return false;
	}
	auto node = static_cast<Node*>(drawableObj);
	if(node->getNodeType() != Node::NodeType::Drawable3D)
	{
		return false;
	}
	auto drawable = static_cast<Drawable3D*>(node);
	return drawable->isOutlineEnabled();
}

bool OutlinePass::buildOutlineQueue(RenderQueue* sourceQueue)
{
	m_outlineQueue.clearCommands();
	if(!sourceQueue)
	{
		return false;
	}

	for(auto& command : sourceQueue->getList())
	{
		if(!isOutlineCommand(command))
		{
			continue;
		}

		auto node = static_cast<Node*>(command.getDrawableObj());
		auto drawable = static_cast<Drawable3D*>(node);
		RenderCommand outlineCommand = command;
		outlineCommand.setMat(m_maskMaterial);
		outlineCommand.setDrawPassMask(DrawPassType::OutlineMask);
		outlineCommand.setOutlineColor(drawable->outlineColor());
		m_outlineQueue.addRenderCommand(outlineCommand, 0);
	}

	return !m_outlineQueue.getList().empty();
}
}
