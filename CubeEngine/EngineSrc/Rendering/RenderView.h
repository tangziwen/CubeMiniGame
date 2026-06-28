#pragma once

#include "Rendering/RenderFlag.h"
#include "Rendering/RenderQueues.h"
#include "Rendering/RenderViewType.h"
#include "Math/Matrix44.h"
#include <vector>

namespace tzw
{
class Camera;
class DeviceRenderCommand;
class DeviceRenderStage;
class RenderPath;

class RenderViewPass
{
public:
	RenderViewPass(DeviceRenderStage* stage, uint32_t renderStageMask, bool consumesSceneQueue);
	DeviceRenderStage* stage() const;
	uint32_t renderStageMask() const;
	bool consumesSceneQueue() const;

private:
	DeviceRenderStage* m_stage;
	uint32_t m_renderStageMask;
	bool m_consumesSceneQueue;
};

class RenderView
{
public:
	explicit RenderView(RenderViewType viewType, int viewIndex = 0);
	virtual ~RenderView() = default;

	RenderViewType viewType() const;
	int viewIndex() const;
	Camera* camera() const;
	RenderQueue* renderQueue();
	const RenderQueue* renderQueue() const;
	uint32_t submitStageMask() const;

	virtual void init() = 0;
	virtual void collect() = 0;
	virtual void draw(DeviceRenderCommand* cmd, RenderPath* renderPath) = 0;

protected:
	void setCamera(Camera* camera);
	void addPass(DeviceRenderStage* stage, uint32_t renderStageMask, bool consumesSceneQueue);
	void clearQueue();
	void applyCameraToCommands(Camera* camera);
	void applyMatricesToCommands(const Matrix44& viewMatrix, const Matrix44& projectMatrix);

	RenderViewType m_viewType;
	int m_viewIndex;
	Camera* m_camera;
	RenderQueue m_renderQueue;
	std::vector<RenderViewPass> m_passes;
};
}
