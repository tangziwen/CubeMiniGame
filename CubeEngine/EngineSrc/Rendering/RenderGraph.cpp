#include "RenderGraph.h"

#include "BackEnd/DeviceMaterial.h"
#include "BackEnd/DeviceFrameBuffer.h"
#include "BackEnd/DevicePipeline.h"
#include "BackEnd/RenderBackEndBase.h"
#include "BackEnd/DeviceRenderStage.h"
#include "Engine/Engine.h"
#include "RenderPath.h"
#include "Utility/log/Log.h"

#include <algorithm>
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
	case RenderGraphResourceLayout::Present:
		return "Present";
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
	case RenderGraphResourceAccessType::ReadWriteDepth:
		return "ReadWriteDepth";
	case RenderGraphResourceAccessType::TransferRead:
		return "TransferRead";
	case RenderGraphResourceAccessType::TransferWrite:
		return "TransferWrite";
	case RenderGraphResourceAccessType::ReadStorageImage:
		return "ReadStorageImage";
	case RenderGraphResourceAccessType::WriteStorageImage:
		return "WriteStorageImage";
	case RenderGraphResourceAccessType::ReadWriteStorageImage:
		return "ReadWriteStorageImage";
	}
	return "Unknown";
}

bool isColorAccess(RenderGraphResourceAccessType type)
{
	return type == RenderGraphResourceAccessType::ReadColor
		|| type == RenderGraphResourceAccessType::WriteColor
		|| type == RenderGraphResourceAccessType::ReadWriteColor
		|| type == RenderGraphResourceAccessType::TransferRead
		|| type == RenderGraphResourceAccessType::TransferWrite
		|| type == RenderGraphResourceAccessType::ReadStorageImage
		|| type == RenderGraphResourceAccessType::WriteStorageImage
		|| type == RenderGraphResourceAccessType::ReadWriteStorageImage;
}

bool isDepthAccess(RenderGraphResourceAccessType type)
{
	return type == RenderGraphResourceAccessType::ReadDepth
		|| type == RenderGraphResourceAccessType::WriteDepth
		|| type == RenderGraphResourceAccessType::ReadWriteDepth;
}

bool isWriteAccess(RenderGraphResourceAccessType type)
{
	return type == RenderGraphResourceAccessType::WriteColor
		|| type == RenderGraphResourceAccessType::WriteDepth
		|| type == RenderGraphResourceAccessType::ReadWriteColor
		|| type == RenderGraphResourceAccessType::ReadWriteDepth
		|| type == RenderGraphResourceAccessType::TransferWrite
		|| type == RenderGraphResourceAccessType::WriteStorageImage
		|| type == RenderGraphResourceAccessType::ReadWriteStorageImage;
}

bool isReadAccess(RenderGraphResourceAccessType type)
{
	return type == RenderGraphResourceAccessType::ReadColor
		|| type == RenderGraphResourceAccessType::ReadDepth
		|| type == RenderGraphResourceAccessType::ReadWriteColor
		|| type == RenderGraphResourceAccessType::ReadWriteDepth
		|| type == RenderGraphResourceAccessType::TransferRead
		|| type == RenderGraphResourceAccessType::ReadStorageImage
		|| type == RenderGraphResourceAccessType::ReadWriteStorageImage;
}

RenderGraphResourceState makeState(RenderGraphResourceLayout layout, RenderGraphResourceUsage usage)
{
	RenderGraphResourceState state;
	state.layout = layout;
	state.usage = usage;
	return state;
}

RenderGraphResourceState stateForLayout(RenderGraphResourceLayout layout, bool)
{
	switch(layout)
	{
	case RenderGraphResourceLayout::ColorAttachment:
		return makeState(layout, RenderGraphResourceUsage::ColorAttachmentWrite);
	case RenderGraphResourceLayout::DepthAttachment:
		return makeState(layout, RenderGraphResourceUsage::DepthAttachmentWrite);
	case RenderGraphResourceLayout::DepthRead:
		return makeState(layout, RenderGraphResourceUsage::FragmentShaderRead);
	case RenderGraphResourceLayout::ShaderRead:
		return makeState(layout, RenderGraphResourceUsage::FragmentShaderRead);
	case RenderGraphResourceLayout::TransferSrc:
		return makeState(layout, RenderGraphResourceUsage::TransferRead);
	case RenderGraphResourceLayout::TransferDst:
		return makeState(layout, RenderGraphResourceUsage::TransferWrite);
	case RenderGraphResourceLayout::General:
		return makeState(layout, RenderGraphResourceUsage::ComputeStorageReadWrite);
	case RenderGraphResourceLayout::Present:
		return makeState(layout, RenderGraphResourceUsage::Present);
	case RenderGraphResourceLayout::Unknown:
		break;
	}
	return makeState(RenderGraphResourceLayout::Unknown, RenderGraphResourceUsage::Unknown);
}

RenderGraphResourceState beforeStateForAccess(const RenderGraphResourceAccess& access)
{
	switch(access.type)
	{
	case RenderGraphResourceAccessType::ReadColor:
		return makeState(RenderGraphResourceLayout::ShaderRead, RenderGraphResourceUsage::FragmentShaderRead);
	case RenderGraphResourceAccessType::ReadDepth:
		return makeState(RenderGraphResourceLayout::DepthRead, RenderGraphResourceUsage::FragmentShaderRead);
	case RenderGraphResourceAccessType::WriteColor:
		return makeState(RenderGraphResourceLayout::ColorAttachment, RenderGraphResourceUsage::ColorAttachmentWrite);
	case RenderGraphResourceAccessType::WriteDepth:
		return makeState(RenderGraphResourceLayout::DepthAttachment, RenderGraphResourceUsage::DepthAttachmentWrite);
	case RenderGraphResourceAccessType::ReadWriteColor:
		return makeState(RenderGraphResourceLayout::ColorAttachment, RenderGraphResourceUsage::ColorAttachmentWrite);
	case RenderGraphResourceAccessType::ReadWriteDepth:
		return makeState(RenderGraphResourceLayout::DepthAttachment, RenderGraphResourceUsage::DepthAttachmentWrite);
	case RenderGraphResourceAccessType::TransferRead:
		return makeState(RenderGraphResourceLayout::TransferSrc, RenderGraphResourceUsage::TransferRead);
	case RenderGraphResourceAccessType::TransferWrite:
		return makeState(RenderGraphResourceLayout::TransferDst, RenderGraphResourceUsage::TransferWrite);
	case RenderGraphResourceAccessType::ReadStorageImage:
		return makeState(RenderGraphResourceLayout::General, RenderGraphResourceUsage::ComputeStorageRead);
	case RenderGraphResourceAccessType::WriteStorageImage:
		return makeState(RenderGraphResourceLayout::General, RenderGraphResourceUsage::ComputeStorageWrite);
	case RenderGraphResourceAccessType::ReadWriteStorageImage:
		return makeState(RenderGraphResourceLayout::General, RenderGraphResourceUsage::ComputeStorageReadWrite);
	}
	return makeState(RenderGraphResourceLayout::Unknown, RenderGraphResourceUsage::Unknown);
}

RenderGraphResourceState afterStateForAccess(const RenderGraphResourceAccess& access)
{
	auto state = beforeStateForAccess(access);
	if(access.afterLayout == RenderGraphResourceLayout::Unknown || access.afterLayout == state.layout)
	{
		return state;
	}
	return stateForLayout(access.afterLayout, isDepthAccess(access.type));
}

bool isUnknownState(const RenderGraphResourceState& state)
{
	return state.layout == RenderGraphResourceLayout::Unknown
		|| state.usage == RenderGraphResourceUsage::Unknown;
}

bool stateReads(RenderGraphResourceUsage usage)
{
	return usage == RenderGraphResourceUsage::FragmentShaderRead
		|| usage == RenderGraphResourceUsage::ComputeStorageRead
		|| usage == RenderGraphResourceUsage::ComputeStorageReadWrite
		|| usage == RenderGraphResourceUsage::TransferRead;
}

bool stateWrites(RenderGraphResourceUsage usage)
{
	return usage == RenderGraphResourceUsage::ComputeStorageWrite
		|| usage == RenderGraphResourceUsage::ComputeStorageReadWrite
		|| usage == RenderGraphResourceUsage::ColorAttachmentWrite
		|| usage == RenderGraphResourceUsage::DepthAttachmentWrite
		|| usage == RenderGraphResourceUsage::TransferWrite;
}

bool needsStateBarrier(const RenderGraphResourceState& currentState, const RenderGraphResourceState& nextState, bool allowSameStateDependency)
{
	if(isUnknownState(currentState) || isUnknownState(nextState))
	{
		return false;
	}
	if(currentState.layout != nextState.layout || currentState.usage != nextState.usage)
	{
		return true;
	}
	return allowSameStateDependency && stateWrites(currentState.usage)
		&& (stateReads(nextState.usage) || stateWrites(nextState.usage));
}

std::vector<DeviceTexture*> texturesForAccess(RenderGraphResource* graphResource, RenderGraphResourceAccessType type)
{
	std::vector<DeviceTexture*> textures;
	if(!graphResource || graphResource->kind != RenderGraphResourceKind::Texture)
	{
		return textures;
	}
	if(isDepthAccess(type))
	{
		if(graphResource->depthTexture)
		{
			textures.emplace_back(graphResource->depthTexture);
		}
		return textures;
	}
	if(graphResource->texture)
	{
		textures.emplace_back(graphResource->texture);
	}
	return textures;
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

std::string rasterPassCacheKey(const RenderGraphRasterPassDesc& desc)
{
	std::ostringstream stream;
	stream << desc.name
		<< "|fb=" << (desc.frameBufferResource.isValid() ? desc.frameBufferResource.index() : invalidResourceIndex())
		<< "|mat=" << reinterpret_cast<uintptr_t>(desc.material)
		<< "|mask=" << desc.drawPassMask
		<< "|consume=" << desc.consumesSceneQueue
		<< "|op=" << static_cast<int>(desc.opType)
		<< "|read=" << desc.isNeedTransitionToRead
		<< "|screen=" << desc.isOutputToScreen;
	for(const auto& attachment : desc.attachments)
	{
		stream << "|att=" << static_cast<int>(attachment.format) << "," << attachment.isDepthStencilAttachment;
	}
	return stream.str();
}

std::string computePassCacheKey(const RenderGraphComputePassDesc& desc)
{
	std::ostringstream stream;
	stream << desc.name
		<< "|shader=" << reinterpret_cast<uintptr_t>(desc.shaderCollection);
	return stream.str();
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

RenderGraphNode::RenderGraphNode()
	: m_graph(nullptr)
	, m_passHandle(RenderGraphPassHandle::invalid())
	, m_output(RenderGraphResourceHandle::invalid())
{
}

RenderGraphNode::RenderGraphNode(RenderGraph* graph, RenderGraphPassHandle passHandle, RenderGraphResourceHandle output)
	: m_graph(graph)
	, m_passHandle(passHandle)
	, m_output(output)
{
}

bool RenderGraphNode::isValid() const
{
	return m_graph && m_passHandle.isValid();
}

RenderGraphPassHandle RenderGraphNode::passHandle() const
{
	return m_passHandle;
}

RenderGraphResourceHandle RenderGraphNode::output() const
{
	return m_output;
}

RenderGraphResourceHandle RenderGraphNode::output(const std::string& name) const
{
	return m_graph ? m_graph->passOutput(m_passHandle, name) : RenderGraphResourceHandle::invalid();
}

RenderGraphNode RenderGraphNode::withOutput(RenderGraphResourceHandle resource) const
{
	return withOutput("primary", resource);
}

RenderGraphNode RenderGraphNode::withOutput(const std::string& name, RenderGraphResourceHandle resource) const
{
	auto node = *this;
	if(name == "primary")
	{
		node.m_output = resource;
	}
	if(node.m_graph)
	{
		node.m_graph->setPassOutput(node.m_passHandle, name, resource);
	}
	return node;
}

RenderGraphNode RenderGraphNode::connect(RenderGraphNode next) const
{
	return next.dependsOn(*this);
}

RenderGraphNode RenderGraphNode::dependsOn(RenderGraphNode dependency) const
{
	auto node = *this;
	if(node.m_graph && dependency.m_graph == node.m_graph)
	{
		node.m_graph->addDependency(node.m_passHandle, dependency.m_passHandle);
	}
	return node;
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

void RenderGraphPassContext::bindSinglePipelineDescriptorCompute()
{
	auto renderStage = stage();
	if(renderStage)
	{
		renderStage->bindSinglePipelineDescriptorCompute();
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

void RenderGraphPassContext::dispatch(uint32_t x, uint32_t y, uint32_t z)
{
	auto renderStage = stage();
	if(renderStage)
	{
		renderStage->dispatch(x, y, z);
	}
}

DeviceRenderStage* RenderGraphPassContext::stage() const
{
	return m_pass ? m_pass->compiledStage : nullptr;
}

RenderGraph::~RenderGraph()
{
	clearResources();
}

void RenderGraph::clear()
{
	m_passes.clear();
	m_compiledPassOrder.clear();
	m_hasCompiledOrder = false;
	m_compiledRoot = RenderGraphPassHandle::invalid();
	m_compileMessage.clear();
}

void RenderGraph::beginBuild()
{
	clear();
}

void RenderGraph::clearResources()
{
	clear();
	releasePassCache();
	releaseOwnedResources();
	m_resources.clear();
	m_resourceRegistrationErrors.clear();
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
		RenderGraphResourceLayout::ColorAttachment, finalLayout));
	return *this;
}

RenderGraphPassDesc& RenderGraphPassDesc::readWriteDepth(RenderGraphResourceHandle resource, RenderGraphResourceLayout finalLayout)
{
	resourceAccesses.emplace_back(makeAccess(resource, RenderGraphResourceAccessType::ReadWriteDepth,
		RenderGraphResourceLayout::DepthAttachment, finalLayout));
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

RenderGraphPassDesc& RenderGraphPassDesc::readStorageImage(RenderGraphResourceHandle resource)
{
	resourceAccesses.emplace_back(makeAccess(resource, RenderGraphResourceAccessType::ReadStorageImage,
		RenderGraphResourceLayout::General, RenderGraphResourceLayout::General));
	return *this;
}

RenderGraphPassDesc& RenderGraphPassDesc::writeStorageImage(RenderGraphResourceHandle resource, RenderGraphResourceLayout finalLayout)
{
	resourceAccesses.emplace_back(makeAccess(resource, RenderGraphResourceAccessType::WriteStorageImage,
		RenderGraphResourceLayout::General, finalLayout));
	return *this;
}

RenderGraphPassDesc& RenderGraphPassDesc::readWriteStorageImage(RenderGraphResourceHandle resource, RenderGraphResourceLayout finalLayout)
{
	resourceAccesses.emplace_back(makeAccess(resource, RenderGraphResourceAccessType::ReadWriteStorageImage,
		RenderGraphResourceLayout::General, finalLayout));
	return *this;
}

RenderGraphPassHandle RenderGraph::addPass(const RenderGraphPassDesc& desc)
{
	auto passDesc = desc;
	const uint32_t index = static_cast<uint32_t>(m_passes.size());
	passDesc.handle = RenderGraphPassHandle(index);
	m_passes.emplace_back(passDesc);
	m_hasCompiledOrder = false;
	m_compiledPassOrder.clear();
	m_compiledRoot = RenderGraphPassHandle::invalid();
	return RenderGraphPassHandle(index);
}

RenderGraphNode RenderGraph::addRasterNode(const RenderGraphRasterPassDesc& desc, std::function<void(RenderGraphPassContext&)> execute)
{
	const auto key = rasterPassCacheKey(desc);
	auto cache = findRasterPassCache(key);
	if(!cache)
	{
		cache = &createRasterPassCache(key, desc, desc.frameBufferResource);
	}

	auto frameBufferResource = cache->frameBufferResource;
	auto frameBuffer = this->frameBuffer(frameBufferResource);
	if(cache->stage)
	{
		cache->stage->setFrameBuffer(frameBuffer);
	}
	auto outputResource = colorAttachment(frameBufferResource);

	RenderGraphPassDesc pass;
	pass.name = desc.name;
	pass.kind = RenderGraphPassKind::Raster;
	pass.renderPass = cache->renderPass;
	pass.compiledStage = cache->stage;
	pass.opType = pass.renderPass ? pass.renderPass->getOpType() : desc.opType;
	pass.frameBufferResource = frameBufferResource;
	pass.outputResource = outputResource;
	pass.isOutputToScreen = pass.renderPass ? pass.renderPass->isOutputToScreen() : desc.isOutputToScreen;
	pass.consumedDrawPassMask = desc.consumesSceneQueue ? desc.drawPassMask : DrawPassType::Unset;
	pass.resourceAccesses = desc.resourceAccesses;
	pass.execute = execute;
	auto handle = addPass(pass);
	if(outputResource.isValid())
	{
		setPassOutput(handle, outputResource);
	}
	return RenderGraphNode(this, handle, outputResource);
}

RenderGraphNode RenderGraph::addFullscreenNode(const RenderGraphRasterPassDesc& desc, std::function<void(RenderGraphPassContext&)> execute)
{
	auto fullscreenDesc = desc;
	fullscreenDesc.consumesSceneQueue = false;
	return addRasterNode(fullscreenDesc, execute);
}

RenderGraphNode RenderGraph::addComputeNode(const RenderGraphComputePassDesc& desc, std::function<void(RenderGraphPassContext&)> execute)
{
	const auto key = computePassCacheKey(desc);
	auto cache = findComputePassCache(key);
	if(!cache)
	{
		cache = &createComputePassCache(key, desc);
	}

	RenderGraphPassDesc pass;
	pass.name = desc.name;
	pass.kind = RenderGraphPassKind::Compute;
	pass.compiledStage = cache->stage;
	pass.resourceAccesses = desc.resourceAccesses;
	pass.execute = execute;
	auto handle = addPass(pass);
	return RenderGraphNode(this, handle, RenderGraphResourceHandle::invalid());
}

RenderGraphNode RenderGraph::addBlitNode(const std::string& name, RenderGraphResourceHandle source, RenderGraphResourceHandle destination)
{
	RenderGraphPassDesc pass;
	pass.name = name;
	pass.kind = RenderGraphPassKind::Blit;
	pass.blitSource = source;
	pass.blitDestination = destination;
	pass.outputResource = destination;
	pass.transferRead(source).transferWrite(destination);
	auto handle = addPass(pass);
	setPassOutput(handle, destination);
	return RenderGraphNode(this, handle, destination);
}

RenderGraphNode RenderGraph::addExternalNode(const RenderGraphPassDesc& desc)
{
	auto pass = desc;
	pass.kind = RenderGraphPassKind::External;
	auto handle = addPass(pass);
	if(pass.outputResource.isValid())
	{
		setPassOutput(handle, pass.outputResource);
	}
	return RenderGraphNode(this, handle, pass.outputResource);
}

RenderGraphPassHandle RenderGraph::addRasterPass(const RenderGraphRasterPassDesc& desc, std::function<void(RenderGraphPassContext&)> execute)
{
	return addRasterNode(desc, execute).passHandle();
}

RenderGraphPassHandle RenderGraph::addFullscreenPass(const RenderGraphRasterPassDesc& desc, std::function<void(RenderGraphPassContext&)> execute)
{
	return addFullscreenNode(desc, execute).passHandle();
}

RenderGraphPassHandle RenderGraph::addComputePass(const RenderGraphComputePassDesc& desc, std::function<void(RenderGraphPassContext&)> execute)
{
	return addComputeNode(desc, execute).passHandle();
}

RenderGraphPassHandle RenderGraph::addBlitPass(const std::string& name, RenderGraphResourceHandle source, RenderGraphResourceHandle destination)
{
	return addBlitNode(name, source, destination).passHandle();
}

RenderGraphPassHandle RenderGraph::addExternalPass(const RenderGraphPassDesc& desc)
{
	return addExternalNode(desc).passHandle();
}

RenderGraph::RasterPassCacheEntry* RenderGraph::findRasterPassCache(const std::string& key)
{
	for(auto& cache : m_rasterPassCache)
	{
		if(cache.key == key)
		{
			return &cache;
		}
	}
	return nullptr;
}

RenderGraph::ComputePassCacheEntry* RenderGraph::findComputePassCache(const std::string& key)
{
	for(auto& cache : m_computePassCache)
	{
		if(cache.key == key)
		{
			return &cache;
		}
	}
	return nullptr;
}

RenderGraph::RasterPassCacheEntry& RenderGraph::createRasterPassCache(const std::string& key, const RenderGraphRasterPassDesc& desc,
	RenderGraphResourceHandle frameBufferResource)
{
	auto backEnd = Engine::shared()->getRenderBackEnd();
	DeviceRenderPass* renderPass = nullptr;
	bool ownsRenderPass = false;
	if(!frameBufferResource.isValid())
	{
		renderPass = backEnd->createDeviceRenderpass_imp();
		if(renderPass)
		{
			renderPass->init(desc.attachments, desc.opType, desc.isNeedTransitionToRead, desc.isOutputToScreen);
			frameBufferResource = createFrameBuffer(desc.frameBufferDesc, renderPass);
			auto graphResource = resource(frameBufferResource);
			if(graphResource)
			{
				graphResource->ownsRenderPass = true;
			}
		}
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
			if(renderPass)
			{
				renderPass->init(desc.attachments, desc.opType, desc.isNeedTransitionToRead, desc.isOutputToScreen);
				ownsRenderPass = true;
			}
		}
	}

	auto frameBuffer = this->frameBuffer(frameBufferResource);
	auto stage = backEnd->createRenderStage_imp();
	if(stage && renderPass && frameBuffer)
	{
		stage->init(renderPass, frameBuffer, desc.drawPassMask);
		stage->setName(desc.name);
		if(desc.material)
		{
			stage->createSinglePipeline(desc.material);
		}
	}

	RasterPassCacheEntry cache;
	cache.key = key;
	cache.renderPass = renderPass;
	cache.stage = stage;
	cache.frameBufferResource = frameBufferResource;
	cache.ownsRenderPass = ownsRenderPass;
	m_rasterPassCache.emplace_back(cache);
	return m_rasterPassCache.back();
}

RenderGraph::ComputePassCacheEntry& RenderGraph::createComputePassCache(const std::string& key, const RenderGraphComputePassDesc& desc)
{
	ComputePassCacheEntry cache;
	cache.key = key;
	if(desc.shaderCollection)
	{
		auto stage = Engine::shared()->getRenderBackEnd()->createRenderStage_imp();
		stage->initCompute();
		stage->setName(desc.name);
		stage->createSingleComputePipeline(desc.shaderCollection);
		cache.stage = stage;
	}
	m_computePassCache.emplace_back(cache);
	return m_computePassCache.back();
}

RenderGraphResourceHandle RenderGraph::findTextureResource(DeviceTexture* texture) const
{
	if(!texture)
	{
		return RenderGraphResourceHandle::invalid();
	}
	for(uint32_t i = 0; i < m_resources.size(); i++)
	{
		const auto& graphResource = m_resources[i];
		if(graphResource.kind == RenderGraphResourceKind::Texture && graphResource.texture == texture)
		{
			return RenderGraphResourceHandle(i);
		}
	}
	return RenderGraphResourceHandle::invalid();
}

RenderGraphResourceHandle RenderGraph::registerTextureResource(const RenderGraphResourceDesc& desc, DeviceTexture* texture, bool imported)
{
	if(!texture)
	{
		const auto error = "RenderGraph cannot register null texture `" + desc.name + "`.";
		m_resourceRegistrationErrors.emplace_back(error);
		tlogError("%s", error.c_str());
		return RenderGraphResourceHandle::invalid();
	}

	auto existingHandle = findTextureResource(texture);
	if(existingHandle.isValid())
	{
		auto existing = resource(existingHandle);
		const bool sizeConflict = existing
			&& existing->desc.size.x > 0 && existing->desc.size.y > 0
			&& desc.size.x > 0 && desc.size.y > 0
			&& (existing->desc.size.x != desc.size.x || existing->desc.size.y != desc.size.y);
		const bool layoutConflict = existing && (desc.role == TextureRoleEnum::AS_DEPTH
			? existing->desc.initialDepthLayout != desc.initialDepthLayout
			: existing->desc.initialColorLayout != desc.initialColorLayout);
		if(!existing || existing->desc.role != desc.role
			|| (existing->desc.format != desc.format && desc.format != ImageFormat::Surface_Format
				&& existing->desc.format != ImageFormat::Surface_Format)
			|| existing->desc.usage != desc.usage
			|| existing->desc.imported != imported
			|| sizeConflict
			|| layoutConflict)
		{
			const auto error = "RenderGraph texture `" + desc.name + "` was imported with conflicting metadata.";
			bool alreadyRecorded = false;
			for(const auto& existingError : m_resourceRegistrationErrors)
			{
				if(existingError == error)
				{
					alreadyRecorded = true;
					break;
				}
			}
			if(!alreadyRecorded)
			{
				m_resourceRegistrationErrors.emplace_back(error);
			}
			tlogError("%s", error.c_str());
			return RenderGraphResourceHandle::invalid();
		}
		return existingHandle;
	}

	RenderGraphResource graphResource;
	graphResource.desc = desc;
	graphResource.desc.imported = imported;
	graphResource.kind = RenderGraphResourceKind::Texture;
	graphResource.texture = texture;
	graphResource.contentsInitialized = imported;
	if(desc.role == TextureRoleEnum::AS_DEPTH)
	{
		graphResource.depthTexture = texture;
		graphResource.currentDepthState = stateForLayout(desc.initialDepthLayout, true);
	}
	else
	{
		graphResource.currentColorState = stateForLayout(desc.initialColorLayout, false);
	}

	const uint32_t index = static_cast<uint32_t>(m_resources.size());
	m_resources.emplace_back(graphResource);
	return RenderGraphResourceHandle(index);
}

void RenderGraph::registerFrameBufferAttachments(RenderGraphResourceHandle frameBufferResource)
{
	auto graphResource = resource(frameBufferResource);
	if(!graphResource || graphResource->kind != RenderGraphResourceKind::FrameBuffer || !graphResource->frameBuffer)
	{
		return;
	}

	graphResource->colorAttachments.clear();
	graphResource->depthAttachment = RenderGraphResourceHandle::invalid();
	auto depthTexture = graphResource->frameBuffer->getDepthMap();
	auto& textures = graphResource->frameBuffer->getTextureList();
	auto attachmentInfos = graphResource->renderPass ? graphResource->renderPass->getAttachmentList() : DeviceAttachmentInfoList{};
	uint32_t colorIndex = 0;
	for(size_t i = 0; i < textures.size(); i++)
	{
		auto texture = textures[i];
		if(!texture)
		{
			continue;
		}
		RenderGraphResourceDesc attachmentDesc = graphResource->desc;
		attachmentDesc.imported = graphResource->desc.imported;
		const bool isDepth = texture == depthTexture;
		if(i < attachmentInfos.size())
		{
			attachmentDesc.format = attachmentInfos[i].format;
		}
		attachmentDesc.role = isDepth ? TextureRoleEnum::AS_DEPTH : TextureRoleEnum::AS_COLOR;
		attachmentDesc.name += isDepth ? ".Depth" : (".Color" + std::to_string(colorIndex));
		auto attachmentHandle = registerTextureResource(attachmentDesc, texture, graphResource->desc.imported);
		if(isDepth)
		{
			graphResource = resource(frameBufferResource);
			graphResource->depthAttachment = attachmentHandle;
		}
		else
		{
			graphResource = resource(frameBufferResource);
			graphResource->colorAttachments.emplace_back(attachmentHandle);
			colorIndex++;
		}
	}
}

RenderGraphResourceHandle RenderGraph::importTexture(const RenderGraphResourceDesc& desc, DeviceTexture* texture)
{
	return registerTextureResource(desc, texture, true);
}

RenderGraphResourceHandle RenderGraph::importFrameBuffer(const RenderGraphResourceDesc& desc, DeviceFrameBuffer* frameBuffer)
{
	RenderGraphResource resource;
	resource.desc = desc;
	resource.desc.imported = true;
	resource.kind = RenderGraphResourceKind::FrameBuffer;
	resource.frameBuffer = frameBuffer;

	const uint32_t index = static_cast<uint32_t>(m_resources.size());
	m_resources.emplace_back(resource);
	RenderGraphResourceHandle handle(index);
	registerFrameBufferAttachments(handle);
	return handle;
}

RenderGraphResourceHandle RenderGraph::createFrameBuffer(const RenderGraphResourceDesc& desc, DeviceRenderPass* renderPass)
{
	auto frameBuffer = Engine::shared()->getRenderBackEnd()->createFrameBuffer_imp();
	frameBuffer->init(static_cast<int>(desc.size.x), static_cast<int>(desc.size.y), renderPass);

	RenderGraphResource resource;
	resource.desc = desc;
	resource.desc.imported = false;
	resource.kind = RenderGraphResourceKind::FrameBuffer;
	resource.frameBuffer = frameBuffer;
	resource.renderPass = renderPass;
	resource.ownsFrameBuffer = true;

	const uint32_t index = static_cast<uint32_t>(m_resources.size());
	m_resources.emplace_back(resource);
	RenderGraphResourceHandle handle(index);
	registerFrameBufferAttachments(handle);
	return handle;
}

RenderGraphResourceHandle RenderGraph::createFrameBuffer(const RenderGraphResourceDesc& desc, const DeviceAttachmentInfoList& attachments,
	DeviceRenderPass::OpType opType, bool isNeedTransitionToRead, bool isOutputToScreen)
{
	auto renderPass = Engine::shared()->getRenderBackEnd()->createDeviceRenderpass_imp();
	renderPass->init(attachments, opType, isNeedTransitionToRead, isOutputToScreen);
	auto handle = createFrameBuffer(desc, renderPass);
	auto graphResource = resource(handle);
	if(graphResource)
	{
		graphResource->ownsRenderPass = true;
	}
	return handle;
}

RenderGraphResourceHandle RenderGraph::colorAttachment(RenderGraphResourceHandle frameBufferResource, uint32_t index) const
{
	auto graphResource = resource(frameBufferResource);
	if(!graphResource || graphResource->kind != RenderGraphResourceKind::FrameBuffer
		|| index >= graphResource->colorAttachments.size())
	{
		return RenderGraphResourceHandle::invalid();
	}
	return graphResource->colorAttachments[index];
}

RenderGraphResourceHandle RenderGraph::depthAttachment(RenderGraphResourceHandle frameBufferResource) const
{
	auto graphResource = resource(frameBufferResource);
	if(!graphResource || graphResource->kind != RenderGraphResourceKind::FrameBuffer)
	{
		return RenderGraphResourceHandle::invalid();
	}
	return graphResource->depthAttachment;
}

bool RenderGraph::rebindImportedFrameBuffer(RenderGraphResourceHandle handle, DeviceFrameBuffer* frameBuffer,
	RenderGraphResourceLayout initialColorLayout, RenderGraphResourceLayout initialDepthLayout)
{
	auto graphResource = resource(handle);
	if(!graphResource || graphResource->kind != RenderGraphResourceKind::FrameBuffer
		|| !graphResource->desc.imported || !frameBuffer)
	{
		return false;
	}

	auto& newTextures = frameBuffer->getTextureList();
	auto newDepth = frameBuffer->getDepthMap();
	if((!graphResource->colorAttachments.empty() && initialColorLayout == RenderGraphResourceLayout::Unknown)
		|| (newDepth && initialDepthLayout == RenderGraphResourceLayout::Unknown))
	{
		tlogError("RenderGraph imported framebuffer `%s` rebind requires explicit attachment states.", graphResource->desc.name.c_str());
		return false;
	}
	std::vector<DeviceTexture*> uniqueTextures;
	for(auto texture : newTextures)
	{
		if(!texture || std::find(uniqueTextures.begin(), uniqueTextures.end(), texture) != uniqueTextures.end())
		{
			tlogError("RenderGraph imported framebuffer `%s` rebind has null or duplicate attachments.", graphResource->desc.name.c_str());
			return false;
		}
		uniqueTextures.emplace_back(texture);
	}
	if(newDepth && std::find(newTextures.begin(), newTextures.end(), newDepth) == newTextures.end())
	{
		tlogError("RenderGraph imported framebuffer `%s` rebind is missing its depth attachment.", graphResource->desc.name.c_str());
		return false;
	}
	size_t newColorCount = 0;
	for(auto texture : newTextures)
	{
		if(texture && texture != newDepth)
		{
			newColorCount++;
		}
	}
	if(newColorCount != graphResource->colorAttachments.size()
		|| static_cast<bool>(newDepth) != graphResource->depthAttachment.isValid())
	{
		tlogError("RenderGraph imported framebuffer `%s` changed attachment shape during rebind.", graphResource->desc.name.c_str());
		return false;
	}
	size_t checkedColorIndex = 0;
	for(auto texture : newTextures)
	{
		if(!texture)
		{
			continue;
		}
		auto expectedHandle = texture == newDepth
			? graphResource->depthAttachment
			: graphResource->colorAttachments[checkedColorIndex++];
		auto attachmentResource = resource(expectedHandle);
		if(!attachmentResource)
		{
			return false;
		}
		if(attachmentResource->texture != texture)
		{
			for(uint32_t resourceIndex = 0; resourceIndex < m_resources.size(); resourceIndex++)
			{
				const auto& other = m_resources[resourceIndex];
				if(resourceIndex == handle.index() || other.kind != RenderGraphResourceKind::FrameBuffer)
				{
					continue;
				}
				bool shared = other.depthAttachment.isValid() && other.depthAttachment.index() == expectedHandle.index();
				for(auto color : other.colorAttachments)
				{
					shared = shared || (color.isValid() && color.index() == expectedHandle.index());
				}
				if(shared)
				{
					tlogError("RenderGraph imported framebuffer `%s` cannot rebind an attachment shared with `%s`.",
						graphResource->desc.name.c_str(), other.desc.name.c_str());
					return false;
				}
			}
		}
		auto existingHandle = findTextureResource(texture);
		if(existingHandle.isValid() && existingHandle.index() != expectedHandle.index())
		{
			tlogError("RenderGraph imported framebuffer `%s` rebind aliases an existing texture resource.", graphResource->desc.name.c_str());
			return false;
		}
	}

	clear();
	invalidateRasterPassCache(handle);
	graphResource->frameBuffer = frameBuffer;
	graphResource->desc.size = frameBuffer->getSize();
	graphResource->desc.initialColorLayout = initialColorLayout;
	graphResource->desc.initialDepthLayout = initialDepthLayout;
	size_t colorIndex = 0;
	for(auto texture : newTextures)
	{
		if(!texture)
		{
			continue;
		}
		if(texture == newDepth)
		{
			auto attachmentResource = resource(graphResource->depthAttachment);
			attachmentResource->texture = texture;
			attachmentResource->depthTexture = texture;
			attachmentResource->desc.size = graphResource->desc.size;
			attachmentResource->contentsInitialized = true;
			attachmentResource->desc.initialDepthLayout = initialDepthLayout;
			attachmentResource->currentDepthState = stateForLayout(initialDepthLayout, true);
		}
		else
		{
			auto attachmentResource = resource(graphResource->colorAttachments[colorIndex++]);
			attachmentResource->texture = texture;
			attachmentResource->desc.size = graphResource->desc.size;
			attachmentResource->contentsInitialized = true;
			attachmentResource->desc.initialColorLayout = initialColorLayout;
			attachmentResource->currentColorState = stateForLayout(initialColorLayout, false);
		}
	}
	return true;
}

void RenderGraph::resizeOwnedFrameBuffers(vec2 size)
{
	clear();
	for(uint32_t resourceIndex = 0; resourceIndex < m_resources.size(); resourceIndex++)
	{
		const auto& graphResource = m_resources[resourceIndex];
		if(graphResource.kind == RenderGraphResourceKind::FrameBuffer && graphResource.ownsFrameBuffer)
		{
			invalidateRasterPassCache(RenderGraphResourceHandle(resourceIndex));
		}
	}
	for(auto& graphResource : m_resources)
	{
		if(graphResource.kind != RenderGraphResourceKind::FrameBuffer
			|| !graphResource.ownsFrameBuffer || !graphResource.renderPass)
		{
			continue;
		}
		delete graphResource.frameBuffer;
		graphResource.frameBuffer = Engine::shared()->getRenderBackEnd()->createFrameBuffer_imp();
		graphResource.frameBuffer->init(static_cast<int>(size.x), static_cast<int>(size.y), graphResource.renderPass);
		graphResource.desc.size = size;
		auto& textures = graphResource.frameBuffer->getTextureList();
		auto depthTexture = graphResource.frameBuffer->getDepthMap();
		size_t colorIndex = 0;
		for(auto texture : textures)
		{
			if(texture == depthTexture)
			{
				auto attachmentResource = resource(graphResource.depthAttachment);
				if(attachmentResource)
				{
					attachmentResource->texture = texture;
					attachmentResource->depthTexture = texture;
					attachmentResource->desc.size = size;
					attachmentResource->contentsInitialized = false;
					attachmentResource->currentDepthState = stateForLayout(graphResource.desc.initialDepthLayout, true);
				}
				continue;
			}
			if(colorIndex < graphResource.colorAttachments.size())
			{
				auto attachmentResource = resource(graphResource.colorAttachments[colorIndex]);
				if(attachmentResource)
				{
					attachmentResource->texture = texture;
					attachmentResource->desc.size = size;
					attachmentResource->contentsInitialized = false;
					attachmentResource->currentColorState = stateForLayout(graphResource.desc.initialColorLayout, false);
				}
			}
			colorIndex++;
		}
	}
}

void RenderGraph::invalidateRasterPassCache(RenderGraphResourceHandle frameBufferResource)
{
	for(auto cache = m_rasterPassCache.begin(); cache != m_rasterPassCache.end();)
	{
		if(!cache->frameBufferResource.isValid() || !frameBufferResource.isValid()
			|| cache->frameBufferResource.index() != frameBufferResource.index())
		{
			++cache;
			continue;
		}
		delete cache->stage;
		cache->stage = nullptr;
		if(cache->ownsRenderPass)
		{
			delete cache->renderPass;
			cache->renderPass = nullptr;
			cache->ownsRenderPass = false;
		}
		cache = m_rasterPassCache.erase(cache);
	}
}

void RenderGraph::setResourceLayout(RenderGraphResourceHandle handle, RenderGraphResourceLayout colorLayout, RenderGraphResourceLayout depthLayout)
{
	auto graphResource = resource(handle);
	if(!graphResource)
	{
		return;
	}
	if(graphResource->kind == RenderGraphResourceKind::FrameBuffer)
	{
		for(auto attachment : graphResource->colorAttachments)
		{
			setResourceLayout(attachment, colorLayout);
		}
		if(graphResource->depthAttachment.isValid())
		{
			setResourceLayout(graphResource->depthAttachment, RenderGraphResourceLayout::Unknown, depthLayout);
		}
		return;
	}
	if(colorLayout != RenderGraphResourceLayout::Unknown && graphResource->desc.role == TextureRoleEnum::AS_COLOR)
	{
		graphResource->currentColorState = stateForLayout(colorLayout, false);
	}
	if(depthLayout != RenderGraphResourceLayout::Unknown && graphResource->desc.role == TextureRoleEnum::AS_DEPTH)
	{
		graphResource->currentDepthState = stateForLayout(depthLayout, true);
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
	return graphResource && graphResource->kind == RenderGraphResourceKind::Texture ? graphResource->texture : nullptr;
}

DeviceTexture* RenderGraph::depthTexture(RenderGraphResourceHandle handle) const
{
	auto graphResource = resource(handle);
	return graphResource && graphResource->kind == RenderGraphResourceKind::Texture ? graphResource->depthTexture : nullptr;
}

DeviceFrameBuffer* RenderGraph::frameBuffer(RenderGraphResourceHandle handle) const
{
	auto graphResource = resource(handle);
	return graphResource && graphResource->kind == RenderGraphResourceKind::FrameBuffer ? graphResource->frameBuffer : nullptr;
}

void RenderGraph::addDependency(RenderGraphPassHandle pass, RenderGraphPassHandle dependency)
{
	if(!pass.isValid() || !dependency.isValid() || pass.index() >= m_passes.size() || dependency.index() >= m_passes.size())
	{
		return;
	}

	auto& dependencies = m_passes[pass.index()].dependencies;
	for(const auto& existing : dependencies)
	{
		if(existing.isValid() && existing.index() == dependency.index())
		{
			return;
		}
	}
	dependencies.emplace_back(dependency);
	m_hasCompiledOrder = false;
	m_compiledPassOrder.clear();
	m_compiledRoot = RenderGraphPassHandle::invalid();
}

void RenderGraph::setPassOutput(RenderGraphPassHandle pass, RenderGraphResourceHandle output)
{
	setPassOutput(pass, "primary", output);
}

void RenderGraph::setPassOutput(RenderGraphPassHandle pass, const std::string& name, RenderGraphResourceHandle output)
{
	if(!pass.isValid() || pass.index() >= m_passes.size())
	{
		return;
	}
	auto& passDesc = m_passes[pass.index()];
	if(name == "primary")
	{
		passDesc.outputResource = output;
	}

	for(auto& namedOutput : passDesc.namedOutputs)
	{
		if(namedOutput.name == name)
		{
			namedOutput.resource = output;
			return;
		}
	}

	RenderGraphPassDesc::NamedOutput namedOutput;
	namedOutput.name = name;
	namedOutput.resource = output;
	passDesc.namedOutputs.emplace_back(namedOutput);
}

RenderGraphResourceHandle RenderGraph::passOutput(RenderGraphPassHandle pass, const std::string& name) const
{
	if(!pass.isValid() || pass.index() >= m_passes.size())
	{
		return RenderGraphResourceHandle::invalid();
	}
	const auto& passDesc = m_passes[pass.index()];
	if(name == "primary")
	{
		return passDesc.outputResource;
	}
	for(const auto& namedOutput : passDesc.namedOutputs)
	{
		if(namedOutput.name == name)
		{
			return namedOutput.resource;
		}
	}
	return RenderGraphResourceHandle::invalid();
}

bool RenderGraph::validate(std::string* outMessage) const
{
	std::ostringstream stream;
	bool isValid = true;
	for(const auto& error : m_resourceRegistrationErrors)
	{
		stream << error << "\n";
		isValid = false;
	}
	for(size_t resourceIndex = 0; resourceIndex < m_resources.size(); resourceIndex++)
	{
		const auto& graphResource = m_resources[resourceIndex];
		if(graphResource.kind == RenderGraphResourceKind::FrameBuffer)
		{
			for(size_t attachmentIndex = 0; attachmentIndex < graphResource.colorAttachments.size(); attachmentIndex++)
			{
				const auto attachment = graphResource.colorAttachments[attachmentIndex];
				if(!resource(attachment))
				{
					stream << "Framebuffer `" << graphResource.desc.name << "` has an invalid color attachment handle.\n";
					isValid = false;
					continue;
				}
				for(size_t otherIndex = attachmentIndex + 1; otherIndex < graphResource.colorAttachments.size(); otherIndex++)
				{
					const auto other = graphResource.colorAttachments[otherIndex];
					if(attachment.isValid() && other.isValid() && attachment.index() == other.index())
					{
						stream << "Framebuffer `" << graphResource.desc.name << "` aliases one texture in multiple attachment slots.\n";
						isValid = false;
					}
				}
			}
			if(graphResource.depthAttachment.isValid() && !resource(graphResource.depthAttachment))
			{
				stream << "Framebuffer `" << graphResource.desc.name << "` has an invalid depth attachment handle.\n";
				isValid = false;
			}
			continue;
		}
		if(graphResource.kind != RenderGraphResourceKind::Texture || !graphResource.texture)
		{
			continue;
		}
		for(size_t otherIndex = resourceIndex + 1; otherIndex < m_resources.size(); otherIndex++)
		{
			const auto& other = m_resources[otherIndex];
			if(other.kind == RenderGraphResourceKind::Texture && other.texture == graphResource.texture)
			{
				stream << "Texture `" << graphResource.desc.name << "` aliases resource `"
					<< other.desc.name << "` through a different handle.\n";
				isValid = false;
			}
		}
	}

	for(const auto& pass : m_passes)
	{
		if(pass.name.empty())
		{
			stream << "RenderGraph contains an unnamed pass.\n";
			isValid = false;
		}
		if(pass.kind == RenderGraphPassKind::Raster)
		{
			auto target = resource(pass.frameBufferResource);
			if(!target || target->kind != RenderGraphResourceKind::FrameBuffer || !target->frameBuffer
				|| !pass.renderPass || !pass.compiledStage)
			{
				stream << "Raster pass `" << pass.name << "` has an invalid framebuffer, render pass, or stage.\n";
				isValid = false;
			}
			else
			{
				auto validateAttachmentAccess = [&](RenderGraphResourceHandle attachment, bool isDepth)
				{
					if(!attachment.isValid())
					{
						return;
					}
					const RenderGraphResourceAccess* found = nullptr;
					for(const auto& access : pass.resourceAccesses)
					{
						if(access.resource.isValid() && access.resource.index() == attachment.index())
						{
							found = &access;
							break;
						}
					}
					const bool validClearAccess = found && (isDepth
						? (found->type == RenderGraphResourceAccessType::WriteDepth || found->type == RenderGraphResourceAccessType::ReadWriteDepth)
						: (found->type == RenderGraphResourceAccessType::WriteColor || found->type == RenderGraphResourceAccessType::ReadWriteColor));
					const bool validLoadAccess = found
						&& (isDepth ? found->type == RenderGraphResourceAccessType::ReadWriteDepth
							: found->type == RenderGraphResourceAccessType::ReadWriteColor);
					if((pass.opType == DeviceRenderPass::OpType::LOADCLEAR_AND_STORE && !validClearAccess)
						|| (pass.opType == DeviceRenderPass::OpType::LOAD_AND_STORE && !validLoadAccess))
					{
						auto attachmentResource = resource(attachment);
						stream << "Raster pass `" << pass.name << "` does not declare a "
							<< (pass.opType == DeviceRenderPass::OpType::LOAD_AND_STORE ? "read-write" : "write")
							<< " access for attachment `"
							<< (attachmentResource ? attachmentResource->desc.name : "<invalid>") << "`.\n";
						isValid = false;
					}
				};
				for(auto color : target->colorAttachments)
				{
					validateAttachmentAccess(color, false);
				}
				validateAttachmentAccess(target->depthAttachment, true);
			}
		}
		else if(pass.kind == RenderGraphPassKind::Compute && !pass.compiledStage)
		{
			stream << "Compute pass `" << pass.name << "` has no compiled stage.\n";
			isValid = false;
		}
		else if(pass.kind == RenderGraphPassKind::Blit
			&& (!texture(pass.blitSource) || !texture(pass.blitDestination)))
		{
			stream << "Blit pass `" << pass.name << "` has invalid texture resources.\n";
			isValid = false;
		}
		for(const auto& dependency : pass.dependencies)
		{
			if(!dependency.isValid() || dependency.index() >= m_passes.size())
			{
				stream << "Pass `" << pass.name << "` references invalid dependency handle.\n";
				isValid = false;
			}
		}
		for(const auto& namedOutput : pass.namedOutputs)
		{
			if(namedOutput.name.empty())
			{
				stream << "Pass `" << pass.name << "` has an unnamed output.\n";
				isValid = false;
			}
			if(!resource(namedOutput.resource))
			{
				stream << "Pass `" << pass.name << "` output `" << namedOutput.name
					<< "` references invalid resource handle.\n";
				isValid = false;
			}
		}
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
			if(graphResource->kind != RenderGraphResourceKind::Texture)
			{
				stream << "Pass `" << pass.name << "` accesses framebuffer `"
					<< graphResource->desc.name << "` instead of one of its attachments.\n";
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
			if(isColorAccess(access.type) && graphResource->desc.role != TextureRoleEnum::AS_COLOR)
			{
				stream << "Pass `" << pass.name << "` uses color access for depth texture `"
					<< graphResource->desc.name << "`.\n";
				isValid = false;
			}
			if(isDepthAccess(access.type) && graphResource->desc.role != TextureRoleEnum::AS_DEPTH)
			{
				stream << "Pass `" << pass.name << "` uses depth access for color texture `"
					<< graphResource->desc.name << "`.\n";
				isValid = false;
			}
			const auto requiredState = beforeStateForAccess(access);
			const auto currentState = isDepthAccess(access.type) ? graphResource->currentDepthState : graphResource->currentColorState;
			if(isUnknownState(currentState))
			{
				stream << "Pass `" << pass.name << "` accesses resource `" << graphResource->desc.name
					<< "` with an unknown initial state.\n";
				isValid = false;
			}
			if(access.beforeLayout != RenderGraphResourceLayout::Unknown && access.beforeLayout != requiredState.layout)
			{
				stream << "Pass `" << pass.name << "` declares an incompatible before layout for `"
					<< graphResource->desc.name << "`.\n";
				isValid = false;
			}
			if(pass.kind == RenderGraphPassKind::External && isWriteAccess(access.type))
			{
				stream << "External pass `" << pass.name << "` declares a GPU write; use a graph-owned pass.\n";
				isValid = false;
			}
			for(size_t j = i + 1; j < pass.resourceAccesses.size(); j++)
			{
				const auto& other = pass.resourceAccesses[j];
				if(other.resource.isValid() && access.resource.isValid()
					&& other.resource.index() == access.resource.index()
					&& (isWriteAccess(access.type) || isWriteAccess(other.type)
						|| access.type != other.type || access.afterLayout != other.afterLayout))
				{
					stream << "Pass `" << pass.name << "` has conflicting accesses for `"
						<< graphResource->desc.name << "`.\n";
					isValid = false;
				}
			}
		}
	}
	if(m_hasCompiledOrder)
	{
		auto hasDependencyPath = [this](size_t passIndex, size_t dependencyIndex)
		{
			std::function<bool(size_t, std::vector<uint8_t>&)> visit =
				[this, dependencyIndex, &visit](size_t currentIndex, std::vector<uint8_t>& visited) -> bool
			{
				if(currentIndex == dependencyIndex)
				{
					return true;
				}
				if(currentIndex >= m_passes.size() || visited[currentIndex])
				{
					return false;
				}
				visited[currentIndex] = 1;
				for(const auto& dependency : m_passes[currentIndex].dependencies)
				{
					if(dependency.isValid() && visit(dependency.index(), visited))
					{
						return true;
					}
				}
				return false;
			};

			std::vector<uint8_t> visited(m_passes.size(), 0);
			return visit(passIndex, visited);
		};

		std::vector<int> lastWriter(m_resources.size(), -1);
		std::vector<std::vector<int>> readersSinceWrite(m_resources.size());
		for(auto passIndex : m_compiledPassOrder)
		{
			if(passIndex >= m_passes.size())
			{
				stream << "Compiled order references invalid pass index `" << passIndex << "`.\n";
				isValid = false;
				continue;
			}
			const auto& pass = m_passes[passIndex];
			for(const auto& access : pass.resourceAccesses)
			{
				if(!access.resource.isValid() || access.resource.index() >= lastWriter.size())
				{
					continue;
				}
				const auto resourceIndex = access.resource.index();
				const auto previousWriter = lastWriter[resourceIndex];
				const auto graphResource = resource(access.resource);
				if(isReadAccess(access.type) && previousWriter < 0 && graphResource
					&& !graphResource->desc.imported && !graphResource->contentsInitialized)
				{
					stream << "Pass `" << pass.name << "` reads graph-owned resource `"
						<< graphResource->desc.name << "` before any reachable writer.\n";
					isValid = false;
				}
				if(previousWriter >= 0 && (isReadAccess(access.type) || isWriteAccess(access.type))
					&& static_cast<size_t>(previousWriter) != passIndex
					&& !hasDependencyPath(passIndex, static_cast<size_t>(previousWriter)))
				{
					const auto& writerPass = m_passes[static_cast<size_t>(previousWriter)];
					stream << "Pass `" << pass.name << "` accesses `"
						<< (graphResource ? graphResource->desc.name : "<invalid>")
						<< "` after writer `" << writerPass.name
						<< "` without a dependency path.\n";
					isValid = false;
				}
				if(isWriteAccess(access.type))
				{
					for(auto readerIndex : readersSinceWrite[resourceIndex])
					{
						if(readerIndex >= 0 && static_cast<size_t>(readerIndex) != passIndex
							&& !hasDependencyPath(passIndex, static_cast<size_t>(readerIndex)))
						{
							stream << "Pass `" << pass.name << "` writes `"
								<< (graphResource ? graphResource->desc.name : "<invalid>")
								<< "` after reader `" << m_passes[static_cast<size_t>(readerIndex)].name
								<< "` without a dependency path.\n";
							isValid = false;
						}
					}
				}
				if(isReadAccess(access.type))
				{
					readersSinceWrite[resourceIndex].emplace_back(static_cast<int>(passIndex));
				}
				if(isWriteAccess(access.type))
				{
					lastWriter[resourceIndex] = static_cast<int>(passIndex);
					readersSinceWrite[resourceIndex].clear();
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
	RenderGraphNode root;
	if(m_hasCompiledOrder && m_compiledRoot.isValid())
	{
		root = RenderGraphNode(const_cast<RenderGraph*>(this), m_compiledRoot, passOutput(m_compiledRoot, "primary"));
	}
	return dump(root);
}

std::string RenderGraph::dump(RenderGraphNode root) const
{
	std::ostringstream stream;
	stream << "RenderGraph:\n";
	RenderGraphPassHandle rootHandle = RenderGraphPassHandle::invalid();
	if(root.isValid() && root.m_graph == this && root.passHandle().index() < m_passes.size())
	{
		rootHandle = root.passHandle();
		stream << "  Root " << m_passes[rootHandle.index()].name << "\n";
	}
	else if(m_hasCompiledOrder && m_compiledRoot.isValid() && m_compiledRoot.index() < m_passes.size())
	{
		rootHandle = m_compiledRoot;
		stream << "  Root " << m_passes[rootHandle.index()].name << "\n";
	}
	else
	{
		stream << "  Root <none>\n";
	}
	stream << dumpCompiledOrder();
	for(size_t passIndex = 0; passIndex < m_passes.size(); passIndex++)
	{
		const auto& pass = m_passes[passIndex];
		stream << "  " << pass.name << ":\n";
		if(rootHandle.isValid() && rootHandle.index() == passIndex)
		{
			stream << "    Root true\n";
		}
		if(!pass.namedOutputs.empty())
		{
			for(const auto& namedOutput : pass.namedOutputs)
			{
				auto output = resource(namedOutput.resource);
				stream << "    Output " << namedOutput.name << " "
					<< (output ? output->desc.name : "<invalid>") << "\n";
			}
		}
		else if(pass.outputResource.isValid())
		{
			auto output = resource(pass.outputResource);
			stream << "    Output primary " << (output ? output->desc.name : "<invalid>") << "\n";
		}
		if(!pass.dependencies.empty())
		{
			stream << "    DependsOn";
			for(const auto& dependency : pass.dependencies)
			{
				stream << " ";
				if(dependency.isValid() && dependency.index() < m_passes.size())
				{
					stream << m_passes[dependency.index()].name;
				}
				else
				{
					stream << "<invalid>";
				}
			}
			stream << "\n";
		}
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

std::string RenderGraph::dumpCompiledOrder() const
{
	std::ostringstream stream;
	stream << "  CompiledOrder";
	if(!m_hasCompiledOrder)
	{
		stream << " <not compiled>\n";
		return stream.str();
	}
	if(m_compiledPassOrder.empty())
	{
		stream << " <empty>\n";
		return stream.str();
	}
	stream << "\n";
	for(size_t i = 0; i < m_compiledPassOrder.size(); i++)
	{
		stream << "    " << i << ": ";
		auto passIndex = m_compiledPassOrder[i];
		if(passIndex < m_passes.size())
		{
			stream << m_passes[passIndex].name;
		}
		else
		{
			stream << "<invalid>";
		}
		stream << "\n";
	}
	return stream.str();
}

std::string RenderGraph::dumpDot() const
{
	RenderGraphNode root;
	if(m_hasCompiledOrder && m_compiledRoot.isValid())
	{
		root = RenderGraphNode(const_cast<RenderGraph*>(this), m_compiledRoot, passOutput(m_compiledRoot, "primary"));
	}
	return dumpDot(root);
}

std::string RenderGraph::dumpDot(RenderGraphNode root) const
{
	auto escape = [](const std::string& text)
	{
		std::string result;
		for(auto ch : text)
		{
			if(ch == '"' || ch == '\\')
			{
				result += '\\';
			}
			result += ch;
		}
		return result;
	};

	RenderGraphPassHandle rootHandle = RenderGraphPassHandle::invalid();
	if(root.isValid() && root.m_graph == this && root.passHandle().index() < m_passes.size())
	{
		rootHandle = root.passHandle();
	}
	else if(m_hasCompiledOrder && m_compiledRoot.isValid() && m_compiledRoot.index() < m_passes.size())
	{
		rootHandle = m_compiledRoot;
	}

	std::ostringstream stream;
	stream << "digraph RenderGraph {\n";
	stream << "  rankdir=LR;\n";
	stream << "  node [fontname=\"Consolas\"];\n";
	for(size_t i = 0; i < m_passes.size(); i++)
	{
		const auto& pass = m_passes[i];
		stream << "  pass_" << i << " [shape=box,label=\"" << i << ": " << escape(pass.name) << "\"";
		if(rootHandle.isValid() && rootHandle.index() == i)
		{
			stream << ",style=filled,fillcolor=\"#ffe6a6\"";
		}
		stream << "];\n";
	}
	for(size_t i = 0; i < m_resources.size(); i++)
	{
		const auto& graphResource = m_resources[i];
		stream << "  resource_" << i << " [shape=ellipse,label=\"" << i << ": "
			<< escape(graphResource.desc.name) << "\"];\n";
	}
	for(size_t i = 0; i < m_passes.size(); i++)
	{
		const auto& pass = m_passes[i];
		for(const auto& dependency : pass.dependencies)
		{
			if(dependency.isValid() && dependency.index() < m_passes.size())
			{
				stream << "  pass_" << dependency.index() << " -> pass_" << i << " [label=\"depends\"];\n";
			}
		}
		for(const auto& namedOutput : pass.namedOutputs)
		{
			if(namedOutput.resource.isValid() && namedOutput.resource.index() < m_resources.size())
			{
				stream << "  pass_" << i << " -> resource_" << namedOutput.resource.index()
					<< " [label=\"output:" << escape(namedOutput.name) << "\",color=\"#3465a4\"];\n";
			}
		}
		for(const auto& access : pass.resourceAccesses)
		{
			if(!access.resource.isValid() || access.resource.index() >= m_resources.size())
			{
				continue;
			}
			const bool writes = isWriteAccess(access.type);
			if(writes)
			{
				stream << "  pass_" << i << " -> resource_" << access.resource.index()
					<< " [label=\"" << accessName(access.type) << "\",style=dashed,color=\"#a40000\"];\n";
			}
			else
			{
				stream << "  resource_" << access.resource.index() << " -> pass_" << i
					<< " [label=\"" << accessName(access.type) << "\",style=dotted,color=\"#4e9a06\"];\n";
			}
		}
	}
	stream << "}\n";
	return stream.str();
}

bool RenderGraph::compile(RenderGraphNode root, std::string* outMessage)
{
	m_compiledPassOrder.clear();
	m_hasCompiledOrder = false;
	m_compiledRoot = RenderGraphPassHandle::invalid();
	m_compileMessage.clear();
	if(!root.isValid() || root.m_graph != this || root.passHandle().index() >= m_passes.size())
	{
		m_compileMessage = "RenderGraph compile failed: invalid root node.\n";
		if(outMessage)
		{
			*outMessage = m_compileMessage;
		}
		tlogError("%s", m_compileMessage.c_str());
		return false;
	}

	std::vector<uint8_t> visitState(m_passes.size(), 0);
	if(!compilePass(root.passHandle(), visitState, m_compileMessage))
	{
		m_compiledPassOrder.clear();
		if(outMessage)
		{
			*outMessage = m_compileMessage;
		}
		tlogError("%s", m_compileMessage.c_str());
		return false;
	}

	m_hasCompiledOrder = true;
	m_compiledRoot = root.passHandle();
	std::string validateMessage;
	if(!validate(&validateMessage))
	{
		m_compileMessage = "RenderGraph compile failed validation:\n" + validateMessage;
		m_compiledPassOrder.clear();
		m_hasCompiledOrder = false;
		m_compiledRoot = RenderGraphPassHandle::invalid();
		if(outMessage)
		{
			*outMessage = m_compileMessage;
		}
		tlogError("%s", m_compileMessage.c_str());
		return false;
	}
	if(outMessage)
	{
		*outMessage = m_compileMessage;
	}
	return true;
}

bool RenderGraph::compilePass(RenderGraphPassHandle pass, std::vector<uint8_t>& visitState, std::string& message)
{
	if(!pass.isValid() || pass.index() >= m_passes.size())
	{
		message += "RenderGraph compile failed: invalid dependency node.\n";
		return false;
	}

	auto index = pass.index();
	if(visitState[index] == 2)
	{
		return true;
	}
	if(visitState[index] == 1)
	{
		message += "RenderGraph compile failed: dependency cycle at pass `" + m_passes[index].name + "`.\n";
		return false;
	}

	visitState[index] = 1;
	for(const auto& dependency : m_passes[index].dependencies)
	{
		if(!compilePass(dependency, visitState, message))
		{
			return false;
		}
	}
	visitState[index] = 2;
	m_compiledPassOrder.emplace_back(index);
	return true;
}

bool RenderGraph::execute(RenderGraphContext& context, std::string* outMessage)
{
	std::string executionMessage;
	if(!m_hasCompiledOrder || !m_compiledRoot.isValid())
	{
		executionMessage = "RenderGraph execute failed: graph has not been compiled successfully.\n";
		if(outMessage)
		{
			*outMessage = executionMessage;
		}
		tlogError("%s", executionMessage.c_str());
		return false;
	}
	std::string validateMessage;
	if(!validate(&validateMessage))
	{
		executionMessage = "RenderGraph validation failed:\n" + validateMessage;
		if(outMessage)
		{
			*outMessage = executionMessage;
		}
		tlogError("%s", executionMessage.c_str());
		return false;
	}

	context.setGraph(this);
	const auto executePass = [this, &context, &executionMessage](RenderGraphPassDesc& pass)
	{
		context.setPass(&pass);
		if(!applyAutomaticTransitions(context, pass, executionMessage))
		{
			return false;
		}
		switch(pass.kind)
		{
		case RenderGraphPassKind::Raster:
			executeRasterPass(context, pass);
			break;
		case RenderGraphPassKind::Compute:
			executeComputePass(context, pass);
			break;
		case RenderGraphPassKind::Blit:
			if(!executeBlitPass(context, pass))
			{
				executionMessage += "Blit pass `" + pass.name + "` failed.\n";
				return false;
			}
			break;
		case RenderGraphPassKind::External:
			executeExternalPass(context, pass);
			break;
		}
		return updateResourceStatesAfterPass(context, pass, executionMessage);
	};

	for(auto passIndex : m_compiledPassOrder)
	{
		if(passIndex >= m_passes.size() || !executePass(m_passes[passIndex]))
		{
			context.setPass(nullptr);
			context.setGraph(nullptr);
			if(outMessage)
			{
				*outMessage = executionMessage;
			}
			tlogError("RenderGraph execution failed:\n%s", executionMessage.c_str());
			return false;
		}
	}
	context.setPass(nullptr);
	context.setGraph(nullptr);
	if(outMessage)
	{
		outMessage->clear();
	}
	return true;
}

void RenderGraph::releasePassCache()
{
	for(auto& cache : m_rasterPassCache)
	{
		delete cache.stage;
		cache.stage = nullptr;
	}
	for(auto& cache : m_computePassCache)
	{
		delete cache.stage;
		cache.stage = nullptr;
	}
}

void RenderGraph::releaseOwnedResources()
{
	for(auto& graphResource : m_resources)
	{
		if(graphResource.kind == RenderGraphResourceKind::FrameBuffer && graphResource.ownsFrameBuffer)
		{
			delete graphResource.frameBuffer;
			graphResource.frameBuffer = nullptr;
			graphResource.ownsFrameBuffer = false;
		}
	}
	for(auto& graphResource : m_resources)
	{
		if(graphResource.kind == RenderGraphResourceKind::FrameBuffer && graphResource.ownsRenderPass)
		{
			delete graphResource.renderPass;
			graphResource.renderPass = nullptr;
			graphResource.ownsRenderPass = false;
		}
	}
	for(auto& cache : m_rasterPassCache)
	{
		if(cache.ownsRenderPass)
		{
			delete cache.renderPass;
			cache.renderPass = nullptr;
			cache.ownsRenderPass = false;
		}
	}
	m_rasterPassCache.clear();
	m_computePassCache.clear();
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

void RenderGraph::executeComputePass(RenderGraphContext& context, RenderGraphPassDesc& pass)
{
	auto stage = pass.compiledStage;
	if(!stage)
	{
		return;
	}

	stage->prepare(context.cmd());
	stage->beginCompute();
	RenderGraphPassContext passContext(&context, &pass);
	if(pass.execute)
	{
		pass.execute(passContext);
	}
	stage->endCompute();
	stage->finish();
	if(context.renderPath())
	{
		context.renderPath()->addRenderStage(stage);
	}
}

bool RenderGraph::executeBlitPass(RenderGraphContext& context, RenderGraphPassDesc& pass)
{
	auto command = context.cmd();
	auto source = texture(pass.blitSource);
	auto destination = texture(pass.blitDestination);
	if(!command || !source || !destination)
	{
		return false;
	}

	auto sourceResource = resource(pass.blitSource);
	auto destinationResource = resource(pass.blitDestination);
	if(!sourceResource || !destinationResource
		|| sourceResource->desc.size.x <= 0 || sourceResource->desc.size.y <= 0
		|| destinationResource->desc.size.x <= 0 || destinationResource->desc.size.y <= 0)
	{
		return false;
	}
	vec2 blitSize((std::min)(sourceResource->desc.size.x, destinationResource->desc.size.x),
		(std::min)(sourceResource->desc.size.y, destinationResource->desc.size.y));

	DeviceTextureBlit blit;
	blit.source = source;
	blit.destination = destination;
	blit.size = blitSize;
	return command->blitTexture(blit);
}

void RenderGraph::executeExternalPass(RenderGraphContext& context, RenderGraphPassDesc& pass)
{
	RenderGraphPassContext passContext(&context, &pass);
	if(pass.execute)
	{
		pass.execute(passContext);
	}
}

bool RenderGraph::applyAutomaticTransitions(RenderGraphContext& context, const RenderGraphPassDesc& pass, std::string& message)
{
	auto command = context.cmd();
	if(!command)
	{
		message += "RenderGraph has no device command for pass `" + pass.name + "`.\n";
		return false;
	}
	for(const auto& access : pass.resourceAccesses)
	{
		auto graphResource = resource(access.resource);
		auto nextState = beforeStateForAccess(access);
		if(!graphResource || isUnknownState(nextState))
		{
			continue;
		}

		auto textures = texturesForAccess(graphResource, access.type);
		if(textures.empty())
		{
			message += "Pass `" + pass.name + "` has no backend texture for `" + graphResource->desc.name + "`.\n";
			return false;
		}

		auto& currentState = isDepthAccess(access.type) ? graphResource->currentDepthState : graphResource->currentColorState;
		auto target = resource(pass.frameBufferResource);
		bool screenColorAccess = false;
		if(pass.isOutputToScreen && target && isColorAccess(access.type) && isWriteAccess(access.type))
		{
			for(auto color : target->colorAttachments)
			{
				if(color.isValid() && access.resource.isValid() && color.index() == access.resource.index())
				{
					screenColorAccess = true;
					break;
				}
			}
		}
		if(isUnknownState(currentState))
		{
			message += "Pass `" + pass.name + "` cannot transition an unknown state for `" + graphResource->desc.name + "`.\n";
			return false;
		}

		if(!screenColorAccess && needsStateBarrier(currentState, nextState, true))
		{
			for(auto texture : textures)
			{
				DeviceTextureBarrier barrier;
				barrier.texture = texture;
				barrier.before = currentState;
				barrier.after = nextState;
				if(!command->textureBarrier(barrier))
				{
					message += "Backend rejected transition for `" + graphResource->desc.name
						+ "` before pass `" + pass.name + "`.\n";
					return false;
				}
			}
		}
		currentState = nextState;
	}
	return true;
}

bool RenderGraph::updateResourceStatesAfterPass(RenderGraphContext& context, const RenderGraphPassDesc& pass, std::string& message)
{
	auto command = context.cmd();
	if(!command)
	{
		message += "RenderGraph has no device command after pass `" + pass.name + "`.\n";
		return false;
	}

	// The backend render pass may have already transitioned its attachments on exit.
	if(pass.kind == RenderGraphPassKind::Raster && pass.renderPass)
	{
		auto target = resource(pass.frameBufferResource);
		if(target)
		{
			for(auto color : target->colorAttachments)
			{
				auto attachment = resource(color);
				if(attachment)
				{
					attachment->currentColorState.layout = pass.renderPass->isNeedTransitionToRead()
						? RenderGraphResourceLayout::ShaderRead
						: (pass.isOutputToScreen ? RenderGraphResourceLayout::Present : RenderGraphResourceLayout::ColorAttachment);
				}
			}
			auto depth = resource(target->depthAttachment);
			if(depth)
			{
				depth->currentDepthState.layout = pass.renderPass->isNeedTransitionToRead()
					? RenderGraphResourceLayout::DepthRead : RenderGraphResourceLayout::DepthAttachment;
			}
		}
	}

	for(const auto& access : pass.resourceAccesses)
	{
		auto nextState = afterStateForAccess(access);
		if(isUnknownState(nextState))
		{
			continue;
		}
		auto graphResource = resource(access.resource);
		if(!graphResource)
		{
			continue;
		}

		auto& currentState = isDepthAccess(access.type) ? graphResource->currentDepthState : graphResource->currentColorState;
		auto target = resource(pass.frameBufferResource);
		bool screenColorAccess = false;
		if(pass.isOutputToScreen && target && isColorAccess(access.type) && isWriteAccess(access.type))
		{
			for(auto color : target->colorAttachments)
			{
				if(color.isValid() && access.resource.isValid() && color.index() == access.resource.index())
				{
					screenColorAccess = true;
					break;
				}
			}
		}
		const bool screenPresentHandoff = screenColorAccess
			&& currentState.layout == RenderGraphResourceLayout::Present && nextState.layout == RenderGraphResourceLayout::Present;
		if(!screenPresentHandoff && !isUnknownState(currentState) && needsStateBarrier(currentState, nextState, false))
		{
			auto textures = texturesForAccess(graphResource, access.type);
			for(auto texture : textures)
			{
				DeviceTextureBarrier barrier;
				barrier.texture = texture;
				barrier.before = currentState;
				barrier.after = nextState;
				if(!command->textureBarrier(barrier))
				{
					message += "Backend rejected transition for `" + graphResource->desc.name
						+ "` after pass `" + pass.name + "`.\n";
					return false;
				}
			}
		}
		currentState = nextState;
		if(isWriteAccess(access.type))
		{
			graphResource->contentsInitialized = true;
		}
	}
	return true;
}
}
