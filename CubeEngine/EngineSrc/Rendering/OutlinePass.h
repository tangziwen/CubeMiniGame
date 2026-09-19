#pragma once

#include "Rendering/RenderQueues.h"

namespace tzw
{
class DeviceRenderCommand;
class DeviceTexture;
class MaterialInstance;
class RenderGraphPassContext;

class OutlinePass
{
public:
	void init();
	bool hasOutlineCommands(const RenderQueue* sourceQueue) const;
	void executeMask(RenderGraphPassContext& graphContext);
	void executeComposite(RenderGraphPassContext& graphContext, DeviceTexture* sceneColor,
		DeviceTexture* maskColor, DeviceTexture* maskDepth, DeviceTexture* sceneDepth);
	MaterialInstance* maskMaterial() const;
	MaterialInstance* compositeMaterial() const;

private:
	bool isOutlineCommand(const RenderCommand& command) const;
	bool buildOutlineQueue(RenderQueue* sourceQueue);

	MaterialInstance* m_maskMaterial = nullptr;
	MaterialInstance* m_compositeMaterial = nullptr;
	RenderQueue m_outlineQueue;
};
}
