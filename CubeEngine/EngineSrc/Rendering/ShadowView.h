#pragma once

#include "Rendering/RenderView.h"

namespace tzw
{
class MaterialInstance;

class ShadowView : public RenderView
{
public:
	ShadowView(RenderGraph& graph, int cascadeIndex = 0);

	void init() override;
	void collect() override;
	RenderGraphNode buildRenderGraph() override;

	int cascadeIndex() const;

private:
	int m_cascadeIndex;
	RenderGraphResourceHandle m_frameBuffer;
	RenderGraphResourceHandle m_depth;
	MaterialInstance* m_shadowMat;
	MaterialInstance* m_shadowInstancedMat;
};
}
