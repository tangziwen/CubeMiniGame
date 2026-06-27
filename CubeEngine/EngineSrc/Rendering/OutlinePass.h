#pragma once

#include "Rendering/RenderQueues.h"

namespace tzw
{
class DeviceRenderCommand;
class DeviceRenderStage;
class DeviceTexture;
class MaterialInstance;
class RenderPath;

class OutlinePass
{
public:
	void init();
	DeviceTexture* draw(DeviceRenderCommand* cmd, RenderPath* renderPath, RenderQueue* sourceQueue,
		DeviceTexture* sceneColor, DeviceTexture* sceneDepth);

private:
	bool buildOutlineQueue(RenderQueue* sourceQueue);

	DeviceRenderStage* m_maskStage = nullptr;
	DeviceRenderStage* m_compositeStage = nullptr;
	MaterialInstance* m_maskMaterial = nullptr;
	MaterialInstance* m_compositeMaterial = nullptr;
	RenderQueue m_outlineQueue;
};
}
