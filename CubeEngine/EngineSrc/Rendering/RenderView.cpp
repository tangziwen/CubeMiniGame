#include "RenderView.h"

#include "Base/Camera.h"

namespace tzw
{
RenderViewPass::RenderViewPass(DeviceRenderStage* stage, uint32_t renderStageMask, bool consumesSceneQueue)
	: m_stage(stage)
	, m_renderStageMask(renderStageMask)
	, m_consumesSceneQueue(consumesSceneQueue)
{
}

DeviceRenderStage* RenderViewPass::stage() const
{
	return m_stage;
}

uint32_t RenderViewPass::renderStageMask() const
{
	return m_renderStageMask;
}

bool RenderViewPass::consumesSceneQueue() const
{
	return m_consumesSceneQueue;
}

RenderView::RenderView(RenderViewType viewType, int viewIndex)
	: m_viewType(viewType)
	, m_viewIndex(viewIndex)
	, m_camera(nullptr)
	, m_submitStageMask(static_cast<uint32_t>(RenderFlag::RenderStage::Unset))
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

uint32_t RenderView::submitStageMask() const
{
	return m_submitStageMask;
}

void RenderView::setCamera(Camera* camera)
{
	m_camera = camera;
}

void RenderView::addPass(DeviceRenderStage* stage, uint32_t renderStageMask, bool consumesSceneQueue)
{
	m_passes.emplace_back(stage, renderStageMask, consumesSceneQueue);
	if(consumesSceneQueue)
	{
		addSubmitStage(renderStageMask);
	}
}

void RenderView::addSubmitStage(uint32_t renderStageMask)
{
	m_submitStageMask |= renderStageMask;
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
