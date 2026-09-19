#include "CSMShadowSystem.h"

#include "3D/ShadowMap/ShadowMap.h"
#include "ShadowView.h"

namespace tzw
{
CSMShadowSystem::CSMShadowSystem()
{
	for(int i = 0; i < SHADOWMAP_CASCADE_NUM; i++)
	{
		m_shadowViews[i] = nullptr;
	}
}

void CSMShadowSystem::init(RenderGraph& graph)
{
	for(int i = 0; i < SHADOWMAP_CASCADE_NUM; i++)
	{
		m_shadowViews[i] = new ShadowView(graph, i);
		m_shadowViews[i]->init();
	}
}

void CSMShadowSystem::collect()
{
	ShadowMap::shared()->calculateProjectionMatrix();
	for(int i = 0; i < SHADOWMAP_CASCADE_NUM; i++)
	{
		m_shadowViews[i]->collect();
	}
}

std::vector<RenderGraphNode> CSMShadowSystem::buildRenderGraph()
{
	std::vector<RenderGraphNode> nodes;
	for(auto view : m_shadowViews)
	{
		nodes.emplace_back(view->buildRenderGraph());
	}
	return nodes;
}
}
