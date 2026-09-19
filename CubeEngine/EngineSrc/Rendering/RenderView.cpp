#include "RenderView.h"

#include "Base/Camera.h"

namespace tzw
{
RenderView::RenderView(RenderGraph& graph, RenderViewType viewType, int viewIndex)
	: m_viewType(viewType)
	, m_viewIndex(viewIndex)
	, m_camera(nullptr)
	, m_submitDrawPassMask(DrawPassType::Unset)
	, m_renderGraph(graph)
{
}

RenderViewType RenderView::viewType() const
{
	return m_viewType;
}

int RenderView::viewIndex() const
{
	return m_viewIndex;
}

Camera* RenderView::camera() const
{
	return m_camera;
}

RenderQueue* RenderView::renderQueue()
{
	return &m_renderQueue;
}

const RenderQueue* RenderView::renderQueue() const
{
	return &m_renderQueue;
}

DrawPassTypeMask RenderView::submitDrawPassMask() const
{
	return m_submitDrawPassMask;
}

void RenderView::setCamera(Camera* camera)
{
	m_camera = camera;
}

void RenderView::addSubmitDrawPass(DrawPassTypeMask drawPassMask)
{
	m_submitDrawPassMask |= drawPassMask;
}

void RenderView::clearQueue()
{
	m_renderQueue.clearCommands();
}

void RenderView::applyCameraToCommands(Camera* camera)
{
	if(!camera)
	{
		return;
	}
	applyMatricesToCommands(camera->getViewMatrix(), camera->projection());
}

void RenderView::applyMatricesToCommands(const Matrix44& viewMatrix, const Matrix44& projectMatrix)
{
	for(auto& command : m_renderQueue.getList())
	{
		command.m_transInfo.m_viewMatrix = viewMatrix;
		command.m_transInfo.m_projectMatrix = projectMatrix;
	}
}
}
