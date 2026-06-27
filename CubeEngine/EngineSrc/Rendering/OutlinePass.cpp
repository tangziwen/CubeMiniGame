#include "OutlinePass.h"

#include "BackEnd/DeviceFrameBuffer.h"
#include "BackEnd/DeviceRenderStage.h"
#include "BackEnd/RenderBackEndBase.h"
#include "Engine/Engine.h"
#include "Interface/Drawable3D.h"
#include "Rendering/ImageFormat.h"
#include "Rendering/RenderCommand.h"
#include "Rendering/RenderFlag.h"
#include "Rendering/RenderPath.h"
#include "Technique/MaterialInstance.h"

namespace tzw
{
void OutlinePass::init()
{
	auto backEnd = Engine::shared()->getRenderBackEnd();
	auto size = Engine::shared()->winSize();

	auto maskPass = backEnd->createDeviceRenderpass_imp();
	maskPass->init({{ImageFormat::R8G8B8A8, false}, {ImageFormat::D24_S8, true}},
		DeviceRenderPass::OpType::LOADCLEAR_AND_STORE, true);
	auto maskBuffer = backEnd->createFrameBuffer_imp();
	maskBuffer->init(size.x, size.y, maskPass);

	m_maskStage = backEnd->createRenderStage_imp();
	m_maskStage->setName("Outline Mask Pass");
	m_maskStage->init(maskPass, maskBuffer, static_cast<uint32_t>(RenderFlag::RenderStage::OUTLINE_MASK));

	m_maskMaterial = new MaterialInstance();
	m_maskMaterial->loadFromMaterial("OutlineMask");

	auto compositePass = backEnd->createDeviceRenderpass_imp();
	compositePass->init({{ImageFormat::R16G16B16A16, false}},
		DeviceRenderPass::OpType::LOADCLEAR_AND_STORE, true);
	auto compositeBuffer = backEnd->createFrameBuffer_imp();
	compositeBuffer->init(size.x, size.y, compositePass);

	m_compositeStage = backEnd->createRenderStage_imp();
	m_compositeStage->setName("Outline Composite Pass");
	m_compositeStage->init(compositePass, compositeBuffer);

	m_compositeMaterial = new MaterialInstance();
	m_compositeMaterial->loadFromMaterial("OutlineComposite");
	m_compositeMaterial->setVar("TU_outlineWidth", 2.0f);
	m_compositeMaterial->setVar("TU_depthEpsilon", 0.0005f);
	m_compositeStage->createSinglePipeline(m_compositeMaterial);
}

DeviceTexture* OutlinePass::draw(DeviceRenderCommand* cmd, RenderPath* renderPath, RenderQueue* sourceQueue,
	DeviceTexture* sceneColor, DeviceTexture* sceneDepth)
{
	if (!m_maskStage || !m_compositeStage)
	{
		init();
	}
	if (!sceneColor || !sceneDepth || !buildOutlineQueue(sourceQueue))
	{
		return sceneColor;
	}

	m_maskStage->prepare(cmd);
	m_maskStage->beginRenderPass(nullptr, vec4(0, 0, 0, 0), vec2(1, 0));
	m_maskStage->draw(&m_outlineQueue, MaterialTechniqueType::Default);
	m_maskStage->endRenderPass();
	m_maskStage->finish();
	renderPath->addRenderStage(m_maskStage);

	auto descriptor = m_compositeStage->getSolorDeviceMaterial()->getMaterialDescriptorSet();
	descriptor->updateDescriptorByBinding(1, sceneColor);
	descriptor->updateDescriptorByBinding(2, m_maskStage->getFrameBuffer()->getTextureList()[0]);
	descriptor->updateDescriptorByBinding(3, m_maskStage->getFrameBuffer()->getDepthMap());
	descriptor->updateDescriptorByBinding(4, sceneDepth);

	m_compositeStage->prepare(cmd);
	m_compositeStage->beginRenderPass();
	m_compositeStage->bindSinglePipelineDescriptor();
	m_compositeStage->drawScreenQuad();
	m_compositeStage->endRenderPass();
	m_compositeStage->finish();
	renderPath->addRenderStage(m_compositeStage);

	return m_compositeStage->getFrameBuffer()->getTextureList()[0];
}

bool OutlinePass::buildOutlineQueue(RenderQueue* sourceQueue)
{
	m_outlineQueue.clearCommands();
	if (!sourceQueue)
	{
		return false;
	}

	for (auto& command : sourceQueue->getList())
	{
		if (command.batchType() != RenderCommand::RenderBatchType::Single || !command.getDrawableObj())
		{
			continue;
		}

		auto node = static_cast<Node*>(command.getDrawableObj());
		if (node->getNodeType() != Node::NodeType::Drawable3D)
		{
			continue;
		}

		auto drawable = static_cast<Drawable3D*>(node);
		if (!drawable->isOutlineEnabled())
		{
			continue;
		}

		RenderCommand outlineCommand = command;
		outlineCommand.setMat(m_maskMaterial);
		outlineCommand.setRenderStageMask(static_cast<uint32_t>(RenderFlag::RenderStage::OUTLINE_MASK));
		outlineCommand.setOutlineColor(drawable->outlineColor());
		m_outlineQueue.addRenderCommand(outlineCommand, 0);
	}

	return !m_outlineQueue.getList().empty();
}
}
