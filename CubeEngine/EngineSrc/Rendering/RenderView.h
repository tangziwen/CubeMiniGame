#pragma once

#include "Rendering/DrawPass.h"
#include "Rendering/RenderFlag.h"
#include "Rendering/RenderQueues.h"
#include "Rendering/RenderViewType.h"
#include "Rendering/RenderGraph.h"
#include "Math/Matrix44.h"
#include <vector>

namespace tzw
{
class Camera;

class RenderView
{
public:
	explicit RenderView(RenderGraph& graph, RenderViewType viewType, int viewIndex = 0);
	virtual ~RenderView() = default;

	RenderViewType viewType() const;
	int viewIndex() const;
	Camera* camera() const;
	RenderQueue* renderQueue();
	const RenderQueue* renderQueue() const;
	DrawPassTypeMask submitDrawPassMask() const;

	virtual void init() = 0;
	virtual void collect() = 0;
	virtual RenderGraphNode buildRenderGraph() = 0;

protected:
	void setCamera(Camera* camera);
	void addSubmitDrawPass(DrawPassTypeMask drawPassMask);
	void clearQueue();
	void applyCameraToCommands(Camera* camera);
	void applyMatricesToCommands(const Matrix44& viewMatrix, const Matrix44& projectMatrix);

	RenderViewType m_viewType;
	int m_viewIndex;
	Camera* m_camera;
	RenderQueue m_renderQueue;
	DrawPassTypeMask m_submitDrawPassMask;
	RenderGraph& m_renderGraph;
};
}
