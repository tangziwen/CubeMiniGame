#include "RenderView.h"

#include "Base/Camera.h"

namespace tzw
{
RenderViewPass::RenderViewPass(DeviceRenderStage* stage, DrawPassTypeMask drawPassMask, bool consumesSceneQueue)
	: m_stage(stage)
	, m_drawPassMask(drawPassMask)
	, m_consumesSceneQueue(consumesSceneQueue)
{
}

DeviceRenderStage* RenderViewPass::stage() const
{
	return m_stage;
}

DrawPassTypeMask RenderViewPass::drawPassMask() const
{
	return m_drawPassMask;
}

bool RenderViewPass::consumesSceneQueue() const
{
	return m_consumesSceneQueue;
}

RenderView::RenderView(RenderViewType viewType, int viewIndex)
	: m_viewType(viewType)
	, m_viewIndex(viewIndex)
	, m_camera(nullptr)
	, m_submitDrawPassMask(DrawPassType::Unset)
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

void RenderView::addPass(DeviceRenderStage* stage, DrawPassTypeMask drawPassMask, bool consumesSceneQueue)
{
	m_passes.emplace_back(stage, drawPassMask, consumesSceneQueue);
	if(consumesSceneQueue)
	{
		addSubmitDrawPass(drawPassMask);
	}
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
