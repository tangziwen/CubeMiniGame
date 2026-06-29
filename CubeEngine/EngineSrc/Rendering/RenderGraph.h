#pragma once

#include <functional>
#include <string>
#include <vector>

#include "Rendering/DrawPass.h"

namespace tzw
{
class DeviceRenderCommand;
class DeviceRenderStage;
class RenderPath;
class RenderQueue;

class RenderGraphContext
{
public:
	RenderGraphContext(DeviceRenderCommand* cmd, RenderPath* renderPath, RenderQueue* sceneQueue);

	DeviceRenderCommand* cmd() const;
	RenderPath* renderPath() const;
	RenderQueue* sceneQueue() const;

private:
	DeviceRenderCommand* m_cmd;
	RenderPath* m_renderPath;
	RenderQueue* m_sceneQueue;
};

struct RenderGraphPassDesc
{
	std::string name;
	DeviceRenderStage* stage = nullptr;
	DrawPassTypeMask consumedDrawPassMask = DrawPassType::Unset;
	std::function<void(RenderGraphContext&)> execute;
};

class RenderGraph
{
public:
	void clear();
	void addPass(const RenderGraphPassDesc& desc);
	void execute(RenderGraphContext& context);

private:
	std::vector<RenderGraphPassDesc> m_passes;
};
}
