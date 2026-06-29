#pragma once

#include "Rendering/DrawPass.h"
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
	RenderViewPass(DeviceRenderStage* stage, DrawPassTypeMask drawPassMask, bool consumesSceneQueue);
	DeviceRenderStage* stage() const;
	DrawPassTypeMask drawPassMask() const;
	bool consumesSceneQueue() const;

private:
	DeviceRenderStage* m_stage;
	DrawPassTypeMask m_drawPassMask;
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
	DrawPassTypeMask submitDrawPassMask() const;

	virtual void init() = 0;
	virtual void collect() = 0;
	virtual void draw(DeviceRenderCommand* cmd, RenderPath* renderPath) = 0;

protected:
	void setCamera(Camera* camera);
	void addPass(DeviceRenderStage* stage, DrawPassTypeMask drawPassMask, bool consumesSceneQueue);
	void addSubmitDrawPass(DrawPassTypeMask drawPassMask);
	void clearQueue();
	void applyCameraToCommands(Camera* camera);
	void applyMatricesToCommands(const Matrix44& viewMatrix, const Matrix44& projectMatrix);

	RenderViewType m_viewType;
	int m_viewIndex;
	Camera* m_camera;
	RenderQueue m_renderQueue;
	DrawPassTypeMask m_submitDrawPassMask;
	std::vector<RenderViewPass> m_passes;
};
}
