#pragma once

#include "3D/ShadowMap/ShadowMap.h"
#include <vector>
#include "RenderGraph.h"

namespace tzw
{
class ShadowView;

class CSMShadowSystem
{
public:
	CSMShadowSystem();

	void init(RenderGraph& graph);
	void collect();
	std::vector<RenderGraphNode> buildRenderGraph();

private:
	ShadowView* m_shadowViews[SHADOWMAP_CASCADE_NUM];
};
}
