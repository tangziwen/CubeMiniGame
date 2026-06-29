#include "RenderGraph.h"

namespace tzw
{
RenderGraphContext::RenderGraphContext(DeviceRenderCommand* cmd, RenderPath* renderPath, RenderQueue* sceneQueue)
	: m_cmd(cmd)
	, m_renderPath(renderPath)
	, m_sceneQueue(sceneQueue)
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

void RenderGraph::clear()
{
	m_passes.clear();
}

void RenderGraph::addPass(const RenderGraphPassDesc& desc)
{
	m_passes.emplace_back(desc);
}

void RenderGraph::execute(RenderGraphContext& context)
{
	for(auto& pass : m_passes)
	{
		if(pass.execute)
		{
			pass.execute(context);
		}
	}
}
}
