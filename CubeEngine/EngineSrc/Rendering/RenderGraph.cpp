#include "RenderGraph.h"

#include "BackEnd/DeviceFrameBuffer.h"
#include "BackEnd/RenderBackEndBase.h"
#include "Engine/Engine.h"
#include "RenderPath.h"

namespace tzw
{
namespace
{
uint32_t invalidResourceIndex()
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

DeviceRenderStage* RenderGraphContext::stage() const
{
	return m_pass ? m_pass->stage : nullptr;
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

void RenderGraph::clear()
{
	m_passes.clear();
}

void RenderGraph::clearResources()
{
	m_resources.clear();
}

void RenderGraph::addPass(const RenderGraphPassDesc& desc)
{
	m_passes.emplace_back(desc);
}

RenderGraphResourceHandle RenderGraph::importTexture(const RenderGraphResourceDesc& desc, DeviceTexture* texture)
{
	RenderGraphResource resource;
	resource.desc = desc;
	resource.desc.imported = true;
	resource.texture = texture;
	resource.depthTexture = nullptr;
	resource.frameBuffer = nullptr;

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

	const uint32_t index = static_cast<uint32_t>(m_resources.size());
	m_resources.emplace_back(resource);
	return RenderGraphResourceHandle(index);
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

void RenderGraph::execute(RenderGraphContext& context)
{
	context.setGraph(this);
	for(auto& pass : m_passes)
	{
		context.setPass(&pass);
		if(pass.execute)
		{
			pass.execute(context);
		}
		if(pass.stage && context.renderPath())
		{
			context.renderPath()->addRenderStage(pass.stage);
		}
	}
	context.setPass(nullptr);
	context.setGraph(nullptr);
}
}
