#include "RenderGraph.h"

#include "BackEnd/DeviceMaterial.h"
#include "BackEnd/DeviceFrameBuffer.h"
#include "BackEnd/DevicePipeline.h"
#include "BackEnd/RenderBackEndBase.h"
#include "BackEnd/DeviceRenderStage.h"
#include "BackEnd/VkRenderBackEnd.h"
#include "BackEnd/vk/DeviceRenderCommandVK.h"
#include "BackEnd/vk/DeviceTextureVK.h"
#include "Engine/Engine.h"
#include "RenderPath.h"
#include "Utility/log/Log.h"

#include <sstream>

namespace tzw
{
namespace
{
uint32_t invalidResourceIndex()
{
	return std::numeric_limits<uint32_t>::max();
}

uint32_t invalidPassIndex()
{
	return std::numeric_limits<uint32_t>::max();
}

DeviceTexture* firstColorTexture(DeviceFrameBuffer* frameBuffer)
{
	if(!frameBuffer)
	{
		return nullptr;
	}

	auto depthTexture = frameBuffer->getDepthMap();
	auto& textures = frameBuffer->getTextureList();
	for(auto texture : textures)
	{
		if(texture && texture != depthTexture)
		{
			return texture;
		}
	}
	return textures.empty() ? nullptr : textures[0];
}

const char* layoutName(RenderGraphResourceLayout layout)
{
	switch(layout)
	{
	case RenderGraphResourceLayout::Unknown:
		return "Unknown";
	case RenderGraphResourceLayout::ColorAttachment:
		return "ColorAttachment";
	case RenderGraphResourceLayout::DepthAttachment:
		return "DepthAttachment";
	case RenderGraphResourceLayout::DepthRead:
		return "DepthRead";
	case RenderGraphResourceLayout::ShaderRead:
		return "ShaderRead";
	case RenderGraphResourceLayout::TransferSrc:
		return "TransferSrc";
	case RenderGraphResourceLayout::TransferDst:
		return "TransferDst";
	case RenderGraphResourceLayout::General:
		return "General";
	}
	return "Unknown";
}

const char* accessName(RenderGraphResourceAccessType type)
{
	switch(type)
	{
	case RenderGraphResourceAccessType::ReadColor:
		return "ReadColor";
	case RenderGraphResourceAccessType::ReadDepth:
		return "ReadDepth";
	case RenderGraphResourceAccessType::WriteColor:
		return "WriteColor";
	case RenderGraphResourceAccessType::WriteDepth:
		return "WriteDepth";
	case RenderGraphResourceAccessType::ReadWriteColor:
		return "ReadWriteColor";
	case RenderGraphResourceAccessType::TransferRead:
		return "TransferRead";
	case RenderGraphResourceAccessType::TransferWrite:
		return "TransferWrite";
	}
	return "Unknown";
}

bool isColorAccess(RenderGraphResourceAccessType type)
{
	return type == RenderGraphResourceAccessType::ReadColor
		|| type == RenderGraphResourceAccessType::WriteColor
		|| type == RenderGraphResourceAccessType::ReadWriteColor
		|| type == RenderGraphResourceAccessType::TransferRead
		|| type == RenderGraphResourceAccessType::TransferWrite;
}

bool isDepthAccess(RenderGraphResourceAccessType type)
{
	return type == RenderGraphResourceAccessType::ReadDepth
		|| type == RenderGraphResourceAccessType::WriteDepth;
}

bool isWriteAccess(RenderGraphResourceAccessType type)
{
	return type == RenderGraphResourceAccessType::WriteColor
		|| type == RenderGraphResourceAccessType::WriteDepth
		|| type == RenderGraphResourceAccessType::ReadWriteColor
		|| type == RenderGraphResourceAccessType::TransferWrite;
}

bool toVkLayout(RenderGraphResourceLayout layout, VkImageLayout& outLayout)
{
	switch(layout)
	{
	case RenderGraphResourceLayout::ColorAttachment:
		outLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		return true;
	case RenderGraphResourceLayout::DepthAttachment:
		outLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		return true;
	case RenderGraphResourceLayout::DepthRead:
		outLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
		return true;
	case RenderGraphResourceLayout::ShaderRead:
		outLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		return true;
	case RenderGraphResourceLayout::TransferSrc:
		outLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		return true;
	case RenderGraphResourceLayout::TransferDst:
		outLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		return true;
	case RenderGraphResourceLayout::General:
		outLayout = VK_IMAGE_LAYOUT_GENERAL;
		return true;
	case RenderGraphResourceLayout::Unknown:
		break;
	}
	return false;
}

RenderGraphResourceAccess makeAccess(RenderGraphResourceHandle resource, RenderGraphResourceAccessType type,
	RenderGraphResourceLayout beforeLayout, RenderGraphResourceLayout afterLayout)
{
	RenderGraphResourceAccess access;
	access.resource = resource;
	access.type = type;
	access.beforeLayout = beforeLayout;
	access.afterLayout = afterLayout;
	return access;
}
}

RenderGraphResourceHandle::RenderGraphResourceHandle()
	: m_index(invalidResourceIndex())
{
}

RenderGraphResourceHandle::RenderGraphResourceHandle(uint32_t index)
	: m_index(index)
{
}

bool RenderGraphResourceHandle::isValid() const
{
	return m_index != invalidResourceIndex();
}

uint32_t RenderGraphResourceHandle::index() const
{
	return m_index;
}

RenderGraphResourceHandle RenderGraphResourceHandle::invalid()
{
	return RenderGraphResourceHandle();
}

RenderGraphPassHandle::RenderGraphPassHandle()
	: m_index(invalidPassIndex())
{
}

RenderGraphPassHandle::RenderGraphPassHandle(uint32_t index)
	: m_index(index)
{
}

bool RenderGraphPassHandle::isValid() const
{
	return m_index != invalidPassIndex();
}

uint32_t RenderGraphPassHandle::index() const
{
	return m_index;
}

RenderGraphPassHandle RenderGraphPassHandle::invalid()
{
	return RenderGraphPassHandle();
}

RenderGraphContext::RenderGraphContext(DeviceRenderCommand* cmd, RenderPath* renderPath, RenderQueue* sceneQueue)
	: m_cmd(cmd)
	, m_renderPath(renderPath)
	, m_sceneQueue(sceneQueue)
	, m_graph(nullptr)
	, m_pass(nullptr)
{
}

DeviceRenderCommand* RenderGraphContext::cmd() const
{
	return m_cmd;
}

RenderPath* RenderGraphContext::renderPath() const
{
	return m_renderPath;
}

RenderQueue* RenderGraphContext::sceneQueue() const
{
	return m_sceneQueue;
}

const RenderGraphPassDesc* RenderGraphContext::pass() const
{
	return m_pass;
}

const RenderGraphResource* RenderGraphContext::resource(RenderGraphResourceHandle handle) const
{
	return m_graph ? m_graph->resource(handle) : nullptr;
}

DeviceTexture* RenderGraphContext::texture(RenderGraphResourceHandle handle) const
{
	return m_graph ? m_graph->texture(handle) : nullptr;
}

DeviceTexture* RenderGraphContext::depthTexture(RenderGraphResourceHandle handle) const
{
	return m_graph ? m_graph->depthTexture(handle) : nullptr;
}

DeviceFrameBuffer* RenderGraphContext::frameBuffer(RenderGraphResourceHandle handle) const
{
	return m_graph ? m_graph->frameBuffer(handle) : nullptr;
}

void RenderGraphContext::setGraph(const RenderGraph* graph)
{
	m_graph = graph;
}

void RenderGraphContext::setPass(const RenderGraphPassDesc* pass)
{
	m_pass = pass;
}

RenderGraphPassContext::RenderGraphPassContext(RenderGraphContext* graphContext, RenderGraphPassDesc* pass)
	: m_graphContext(graphContext)
	, m_pass(pass)
{
}

DeviceRenderCommand* RenderGraphPassContext::cmd() const
{
	return m_graphContext ? m_graphContext->cmd() : nullptr;
}

RenderPath* RenderGraphPassContext::renderPath() const
{
	return m_graphContext ? m_graphContext->renderPath() : nullptr;
}

RenderQueue* RenderGraphPassContext::sceneQueue() const
{
	return m_graphContext ? m_graphContext->sceneQueue() : nullptr;
}

const RenderGraphPassDesc* RenderGraphPassContext::pass() const
{
	return m_pass;
}

const RenderGraphResource* RenderGraphPassContext::resource(RenderGraphResourceHandle handle) const
{
	return m_graphContext ? m_graphContext->resource(handle) : nullptr;
}

DeviceTexture* RenderGraphPassContext::texture(RenderGraphResourceHandle handle) const
{
	return m_graphContext ? m_graphContext->texture(handle) : nullptr;
}

DeviceTexture* RenderGraphPassContext::depthTexture(RenderGraphResourceHandle handle) const
{
	return m_graphContext ? m_graphContext->depthTexture(handle) : nullptr;
}

DeviceFrameBuffer* RenderGraphPassContext::frameBuffer(RenderGraphResourceHandle handle) const
{
	return m_graphContext ? m_graphContext->frameBuffer(handle) : nullptr;
}

DeviceFrameBuffer* RenderGraphPassContext::targetFrameBuffer() const
{
	return m_pass && m_graphContext ? m_graphContext->frameBuffer(m_pass->frameBufferResource) : nullptr;
}

DeviceMaterial* RenderGraphPassContext::material() const
{
	auto renderStage = stage();
	return renderStage ? renderStage->getSolorDeviceMaterial() : nullptr;
}

DeviceDescriptor* RenderGraphPassContext::materialDescriptor() const
{
	auto deviceMaterial = material();
	return deviceMaterial ? deviceMaterial->getMaterialDescriptorSet() : nullptr;
}

DevicePipeline* RenderGraphPassContext::pipeline() const
{
	auto renderStage = stage();
	return renderStage ? renderStage->getSinglePipeline() : nullptr;
}

DeviceDescriptor* RenderGraphPassContext::itemDescriptor() const
{
	auto devicePipeline = pipeline();
	return devicePipeline ? devicePipeline->giveItemWiseDescriptorSet() : nullptr;
}

void RenderGraphPassContext::bindSinglePipelineDescriptor()
{
	auto renderStage = stage();
	if(renderStage)
	{
		renderStage->bindSinglePipelineDescriptor();
	}
}

void RenderGraphPassContext::bindSinglePipelineDescriptor(DeviceDescriptor* extraItemDescriptor)
{
	auto renderStage = stage();
	if(renderStage)
	{
		renderStage->bindSinglePipelineDescriptor(extraItemDescriptor);
	}
}

void RenderGraphPassContext::drawQueue(RenderQueue* renderQueue, MaterialTechniqueType techniqueType)
{
	auto renderStage = stage();
	if(renderStage)
	{
		renderStage->draw(renderQueue, techniqueType);
	}
}

void RenderGraphPassContext::drawSceneQueue(MaterialTechniqueType techniqueType)
{
	drawQueue(sceneQueue(), techniqueType);
}

void RenderGraphPassContext::drawScreenQuad()
{
	auto renderStage = stage();
	if(renderStage)
	{
		renderStage->drawScreenQuad();
	}
}

void RenderGraphPassContext::drawSphere()
{
	auto renderStage = stage();
	if(renderStage)
	{
		renderStage->drawSphere();
	}
}

DeviceRenderStage* RenderGraphPassContext::stage() const
{
	return m_pass ? m_pass->compiledStage : nullptr;
}

void RenderGraph::clear()
{
	m_passes.clear();
}

void RenderGraph::clearResources()
{
	releaseOwnedResources();
	m_resources.clear();
}

RenderGraphPassDesc& RenderGraphPassDesc::readColor(RenderGraphResourceHandle resource)
{
	resourceAccesses.emplace_back(makeAccess(resource, RenderGraphResourceAccessType::ReadColor,
		RenderGraphResourceLayout::ShaderRead, RenderGraphResourceLayout::ShaderRead));
	return *this;
}

RenderGraphPassDesc& RenderGraphPassDesc::readDepth(RenderGraphResourceHandle resource)
{
	resourceAccesses.emplace_back(makeAccess(resource, RenderGraphResourceAccessType::ReadDepth,
		RenderGraphResourceLayout::DepthRead, RenderGraphResourceLayout::DepthRead));
	return *this;
}

RenderGraphPassDesc& RenderGraphPassDesc::writeColor(RenderGraphResourceHandle resource, RenderGraphResourceLayout finalLayout)
{
	resourceAccesses.emplace_back(makeAccess(resource, RenderGraphResourceAccessType::WriteColor,
		RenderGraphResourceLayout::ColorAttachment, finalLayout));
	return *this;
}

RenderGraphPassDesc& RenderGraphPassDesc::writeDepth(RenderGraphResourceHandle resource, RenderGraphResourceLayout finalLayout)
{
	resourceAccesses.emplace_back(makeAccess(resource, RenderGraphResourceAccessType::WriteDepth,
		RenderGraphResourceLayout::DepthAttachment, finalLayout));
	return *this;
}

RenderGraphPassDesc& RenderGraphPassDesc::readWriteColor(RenderGraphResourceHandle resource, RenderGraphResourceLayout finalLayout)
{
	resourceAccesses.emplace_back(makeAccess(resource, RenderGraphResourceAccessType::ReadWriteColor,
		RenderGraphResourceLayout::Unknown, finalLayout));
	return *this;
}

RenderGraphPassDesc& RenderGraphPassDesc::transferRead(RenderGraphResourceHandle resource)
{
	resourceAccesses.emplace_back(makeAccess(resource, RenderGraphResourceAccessType::TransferRead,
		RenderGraphResourceLayout::TransferSrc, RenderGraphResourceLayout::TransferSrc));
	return *this;
}

RenderGraphPassDesc& RenderGraphPassDesc::transferWrite(RenderGraphResourceHandle resource)
{
	resourceAccesses.emplace_back(makeAccess(resource, RenderGraphResourceAccessType::TransferWrite,
		RenderGraphResourceLayout::TransferDst, RenderGraphResourceLayout::TransferDst));
	return *this;
}

RenderGraphPassHandle RenderGraph::addPass(const RenderGraphPassDesc& desc)
{
	auto passDesc = desc;
	const uint32_t index = static_cast<uint32_t>(m_passes.size());
	passDesc.handle = RenderGraphPassHandle(index);
	m_passes.emplace_back(passDesc);
	return RenderGraphPassHandle(index);
}

RenderGraphPassHandle RenderGraph::addRasterPass(const RenderGraphRasterPassDesc& desc, std::function<void(RenderGraphPassContext&)> execute)
{
	auto backEnd = Engine::shared()->getRenderBackEnd();
	auto frameBufferResource = desc.frameBufferResource;
	DeviceRenderPass* renderPass = nullptr;
	if(!frameBufferResource.isValid())
	{
		renderPass = backEnd->createDeviceRenderpass_imp();
		renderPass->init(desc.attachments, desc.opType, desc.isNeedTransitionToRead, desc.isOutputToScreen);
		frameBufferResource = createFrameBuffer(desc.frameBufferDesc, renderPass);
	}
	else
	{
		auto graphResource = resource(frameBufferResource);
		if(desc.attachments.empty() && graphResource)
		{
			renderPass = graphResource->renderPass;
		}
		if(!renderPass)
		{
			renderPass = backEnd->createDeviceRenderpass_imp();
			renderPass->init(desc.attachments, desc.opType, desc.isNeedTransitionToRead, desc.isOutputToScreen);
		}
	}
	auto frameBuffer = this->frameBuffer(frameBufferResource);

	auto stage = backEnd->createRenderStage_imp();
	stage->init(renderPass, frameBuffer, desc.drawPassMask);
	stage->setName(desc.name);
	if(desc.material)
	{
		stage->createSinglePipeline(desc.material);
	}

	RenderGraphPassDesc pass;
	pass.name = desc.name;
	pass.kind = RenderGraphPassKind::Raster;
	pass.renderPass = renderPass;
	pass.compiledStage = stage;
	pass.frameBufferResource = frameBufferResource;
	pass.consumedDrawPassMask = desc.consumesSceneQueue ? desc.drawPassMask : DrawPassType::Unset;
	pass.resourceAccesses = desc.resourceAccesses;
	pass.execute = execute;
	return addPass(pass);
}

RenderGraphPassHandle RenderGraph::addFullscreenPass(const RenderGraphRasterPassDesc& desc, std::function<void(RenderGraphPassContext&)> execute)
{
	auto fullscreenDesc = desc;
	fullscreenDesc.consumesSceneQueue = false;
	return addRasterPass(fullscreenDesc, execute);
}

RenderGraphPassHandle RenderGraph::addBlitPass(const std::string& name, RenderGraphResourceHandle source, RenderGraphResourceHandle destination)
{
	RenderGraphPassDesc pass;
	pass.name = name;
	pass.kind = RenderGraphPassKind::Blit;
	pass.blitSource = source;
	pass.blitDestination = destination;
	pass.transferRead(source).transferWrite(destination);
	return addPass(pass);
}

RenderGraphPassHandle RenderGraph::addExternalPass(const RenderGraphPassDesc& desc)
{
	auto pass = desc;
	pass.kind = RenderGraphPassKind::External;
	return addPass(pass);
}

RenderGraphResourceHandle RenderGraph::importTexture(const RenderGraphResourceDesc& desc, DeviceTexture* texture)
{
	RenderGraphResource resource;
	resource.desc = desc;
	resource.desc.imported = true;
	resource.texture = texture;
	resource.depthTexture = nullptr;
	resource.frameBuffer = nullptr;
	resource.renderPass = nullptr;
	resource.graphOwned = false;
	resource.currentColorLayout = desc.initialColorLayout;
	resource.currentDepthLayout = desc.initialDepthLayout;

	const uint32_t index = static_cast<uint32_t>(m_resources.size());
	m_resources.emplace_back(resource);
	return RenderGraphResourceHandle(index);
}

RenderGraphResourceHandle RenderGraph::importFrameBuffer(const RenderGraphResourceDesc& desc, DeviceFrameBuffer* frameBuffer)
{
	RenderGraphResource resource;
	resource.desc = desc;
	resource.desc.imported = true;
	resource.texture = firstColorTexture(frameBuffer);
	resource.depthTexture = frameBuffer ? frameBuffer->getDepthMap() : nullptr;
	resource.frameBuffer = frameBuffer;
	resource.renderPass = nullptr;
	resource.graphOwned = false;
	resource.currentColorLayout = desc.initialColorLayout;
	resource.currentDepthLayout = desc.initialDepthLayout;

	const uint32_t index = static_cast<uint32_t>(m_resources.size());
	m_resources.emplace_back(resource);
	return RenderGraphResourceHandle(index);
}

RenderGraphResourceHandle RenderGraph::createFrameBuffer(const RenderGraphResourceDesc& desc, DeviceRenderPass* renderPass)
{
	auto frameBuffer = Engine::shared()->getRenderBackEnd()->createFrameBuffer_imp();
	frameBuffer->init(static_cast<int>(desc.size.x), static_cast<int>(desc.size.y), renderPass);

	RenderGraphResource resource;
	resource.desc = desc;
	resource.desc.imported = false;
	resource.texture = firstColorTexture(frameBuffer);
	resource.depthTexture = frameBuffer->getDepthMap();
	resource.frameBuffer = frameBuffer;
	resource.renderPass = renderPass;
	resource.graphOwned = true;
	resource.currentColorLayout = desc.initialColorLayout;
	resource.currentDepthLayout = desc.initialDepthLayout;

	const uint32_t index = static_cast<uint32_t>(m_resources.size());
	m_resources.emplace_back(resource);
	return RenderGraphResourceHandle(index);
}

RenderGraphResourceHandle RenderGraph::createFrameBuffer(const RenderGraphResourceDesc& desc, const DeviceAttachmentInfoList& attachments,
	DeviceRenderPass::OpType opType, bool isNeedTransitionToRead, bool isOutputToScreen)
{
	auto renderPass = Engine::shared()->getRenderBackEnd()->createDeviceRenderpass_imp();
	renderPass->init(attachments, opType, isNeedTransitionToRead, isOutputToScreen);
	return createFrameBuffer(desc, renderPass);
}

void RenderGraph::resizeOwnedFrameBuffers(vec2 size)
{
	for(auto& graphResource : m_resources)
	{
		if(!graphResource.graphOwned || !graphResource.renderPass)
		{
			continue;
		}
		delete graphResource.frameBuffer;
		graphResource.frameBuffer = Engine::shared()->getRenderBackEnd()->createFrameBuffer_imp();
		graphResource.frameBuffer->init(static_cast<int>(size.x), static_cast<int>(size.y), graphResource.renderPass);
		graphResource.desc.size = size;
		graphResource.texture = firstColorTexture(graphResource.frameBuffer);
		graphResource.depthTexture = graphResource.frameBuffer->getDepthMap();
		graphResource.currentColorLayout = graphResource.desc.initialColorLayout;
		graphResource.currentDepthLayout = graphResource.desc.initialDepthLayout;
	}
}

void RenderGraph::setResourceLayout(RenderGraphResourceHandle handle, RenderGraphResourceLayout colorLayout, RenderGraphResourceLayout depthLayout)
{
	auto graphResource = resource(handle);
	if(!graphResource)
	{
		return;
	}
	if(colorLayout != RenderGraphResourceLayout::Unknown)
	{
		graphResource->currentColorLayout = colorLayout;
	}
	if(depthLayout != RenderGraphResourceLayout::Unknown)
	{
		graphResource->currentDepthLayout = depthLayout;
	}
}

DeviceFrameBuffer* RenderGraph::passFrameBuffer(RenderGraphPassHandle handle) const
{
	if(!handle.isValid() || handle.index() >= m_passes.size())
	{
		return nullptr;
	}
	return frameBuffer(m_passes[handle.index()].frameBufferResource);
}

RenderGraphResource* RenderGraph::resource(RenderGraphResourceHandle handle)
{
	if(!handle.isValid() || handle.index() >= m_resources.size())
	{
		return nullptr;
	}
	return &m_resources[handle.index()];
}

const RenderGraphResource* RenderGraph::resource(RenderGraphResourceHandle handle) const
{
	if(!handle.isValid() || handle.index() >= m_resources.size())
	{
		return nullptr;
	}
	return &m_resources[handle.index()];
}

DeviceTexture* RenderGraph::texture(RenderGraphResourceHandle handle) const
{
	auto graphResource = resource(handle);
	return graphResource ? graphResource->texture : nullptr;
}

DeviceTexture* RenderGraph::depthTexture(RenderGraphResourceHandle handle) const
{
	auto graphResource = resource(handle);
	return graphResource ? graphResource->depthTexture : nullptr;
}

DeviceFrameBuffer* RenderGraph::frameBuffer(RenderGraphResourceHandle handle) const
{
	auto graphResource = resource(handle);
	return graphResource ? graphResource->frameBuffer : nullptr;
}

bool RenderGraph::validate(std::string* outMessage) const
{
	std::ostringstream stream;
	bool isValid = true;
	for(const auto& pass : m_passes)
	{
		for(size_t i = 0; i < pass.resourceAccesses.size(); i++)
		{
			const auto& access = pass.resourceAccesses[i];
			auto graphResource = resource(access.resource);
			if(!graphResource)
			{
				stream << "Pass `" << pass.name << "` references invalid resource handle.\n";
				isValid = false;
				continue;
			}
			if(isColorAccess(access.type) && !graphResource->texture)
			{
				stream << "Pass `" << pass.name << "` " << accessName(access.type)
					<< " has no color texture for `" << graphResource->desc.name << "`.\n";
				isValid = false;
			}
			if(isDepthAccess(access.type) && !graphResource->depthTexture)
			{
				stream << "Pass `" << pass.name << "` " << accessName(access.type)
					<< " has no depth texture for `" << graphResource->desc.name << "`.\n";
				isValid = false;
			}
			for(size_t j = i + 1; j < pass.resourceAccesses.size(); j++)
			{
				const auto& other = pass.resourceAccesses[j];
				if(other.resource.isValid() && access.resource.isValid()
					&& other.resource.index() == access.resource.index()
					&& ((isColorAccess(access.type) && isColorAccess(other.type))
						|| (isDepthAccess(access.type) && isDepthAccess(other.type)))
					&& (isWriteAccess(access.type) || isWriteAccess(other.type))
					&& access.type != RenderGraphResourceAccessType::ReadWriteColor
					&& other.type != RenderGraphResourceAccessType::ReadWriteColor)
				{
					stream << "Pass `" << pass.name << "` has conflicting accesses for `"
						<< graphResource->desc.name << "`.\n";
					isValid = false;
				}
			}
		}
	}
	if(outMessage)
	{
		*outMessage = stream.str();
	}
	return isValid;
}

std::string RenderGraph::dump() const
{
	std::ostringstream stream;
	stream << "RenderGraph:\n";
	for(const auto& pass : m_passes)
	{
		stream << "  " << pass.name << ":\n";
		for(const auto& access : pass.resourceAccesses)
		{
			auto graphResource = resource(access.resource);
			stream << "    " << accessName(access.type) << " "
				<< (graphResource ? graphResource->desc.name : "<invalid>")
				<< " before=" << layoutName(access.beforeLayout)
				<< " after=" << layoutName(access.afterLayout) << "\n";
		}
	}
	return stream.str();
}

void RenderGraph::execute(RenderGraphContext& context)
{
	std::string validateMessage;
	if(!validate(&validateMessage))
	{
		tlogError("RenderGraph validation failed:\n%s", validateMessage.c_str());
	}

	context.setGraph(this);
	for(auto& pass : m_passes)
	{
		context.setPass(&pass);
		applyAutomaticTransitions(context, pass);
		switch(pass.kind)
		{
		case RenderGraphPassKind::Raster:
			executeRasterPass(context, pass);
			break;
		case RenderGraphPassKind::Blit:
			executeBlitPass(context, pass);
			break;
		case RenderGraphPassKind::External:
			executeExternalPass(context, pass);
			break;
		}
		updateResourceLayoutsAfterPass(pass);
	}
	context.setPass(nullptr);
	context.setGraph(nullptr);
}

void RenderGraph::releaseOwnedResources()
{
	for(auto& graphResource : m_resources)
	{
		if(graphResource.graphOwned)
		{
			delete graphResource.frameBuffer;
			graphResource.frameBuffer = nullptr;
			graphResource.texture = nullptr;
			graphResource.depthTexture = nullptr;
		}
	}
}

void RenderGraph::executeRasterPass(RenderGraphContext& context, RenderGraphPassDesc& pass)
{
	auto stage = pass.compiledStage;
	if(!stage)
	{
		return;
	}

	stage->prepare(context.cmd());
	stage->beginRenderPass();
	RenderGraphPassContext passContext(&context, &pass);
	if(pass.execute)
	{
		pass.execute(passContext);
	}
	stage->endRenderPass();
	stage->finish();
	if(context.renderPath())
	{
		context.renderPath()->addRenderStage(stage);
	}
}

void RenderGraph::executeBlitPass(RenderGraphContext& context, RenderGraphPassDesc& pass)
{
	auto backEnd = static_cast<VKRenderBackEnd*>(Engine::shared()->getRenderBackEnd());
	auto command = dynamic_cast<DeviceRenderCommandVK*>(context.cmd());
	auto source = dynamic_cast<DeviceTextureVK*>(texture(pass.blitSource));
	auto destination = dynamic_cast<DeviceTextureVK*>(texture(pass.blitDestination));
	if(!backEnd || !command || !source || !destination)
	{
		return;
	}

	auto sourceFrameBuffer = frameBuffer(pass.blitSource);
	auto destinationFrameBuffer = frameBuffer(pass.blitDestination);
	vec2 blitSize = sourceFrameBuffer ? sourceFrameBuffer->getSize() : vec2(0, 0);
	if(blitSize.x <= 0 || blitSize.y <= 0)
	{
		blitSize = destinationFrameBuffer ? destinationFrameBuffer->getSize() : vec2(0, 0);
	}
	if(blitSize.x <= 0 || blitSize.y <= 0)
	{
		return;
	}

	backEnd->blitTexture(command->getVK(), source, destination, blitSize,
		VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
}

void RenderGraph::executeExternalPass(RenderGraphContext& context, RenderGraphPassDesc& pass)
{
	RenderGraphPassContext passContext(&context, &pass);
	if(pass.execute)
	{
		pass.execute(passContext);
	}
}

void RenderGraph::applyAutomaticTransitions(RenderGraphContext& context, const RenderGraphPassDesc& pass)
{
	auto command = dynamic_cast<DeviceRenderCommandVK*>(context.cmd());
	if(!command)
	{
		return;
	}

	auto backEnd = static_cast<VKRenderBackEnd*>(Engine::shared()->getRenderBackEnd());
	for(const auto& access : pass.resourceAccesses)
	{
		auto graphResource = resource(access.resource);
		if(!graphResource || access.beforeLayout == RenderGraphResourceLayout::Unknown)
		{
			continue;
		}

		auto texture = isDepthAccess(access.type) ? graphResource->depthTexture : graphResource->texture;
		auto vkTexture = dynamic_cast<DeviceTextureVK*>(texture);
		if(!vkTexture)
		{
			continue;
		}

		auto& currentLayout = isDepthAccess(access.type) ? graphResource->currentDepthLayout : graphResource->currentColorLayout;
		if(currentLayout == RenderGraphResourceLayout::Unknown || currentLayout == access.beforeLayout)
		{
			currentLayout = access.beforeLayout;
			continue;
		}

		VkImageLayout oldLayout;
		VkImageLayout newLayout;
		if(toVkLayout(currentLayout, oldLayout) && toVkLayout(access.beforeLayout, newLayout))
		{
			backEnd->transitionImageLayoutUseBarrier(command->getVK(), vkTexture, oldLayout, newLayout, 0, 1);
			currentLayout = access.beforeLayout;
		}
	}
}

void RenderGraph::updateResourceLayoutsAfterPass(const RenderGraphPassDesc& pass)
{
	for(const auto& access : pass.resourceAccesses)
	{
		if(access.afterLayout == RenderGraphResourceLayout::Unknown)
		{
			continue;
		}
		auto graphResource = resource(access.resource);
		if(!graphResource)
		{
			continue;
		}
		if(isDepthAccess(access.type))
		{
			graphResource->currentDepthLayout = access.afterLayout;
		}
		else
		{
			graphResource->currentColorLayout = access.afterLayout;
		}
	}
}
}
