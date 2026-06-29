#pragma once

#include <cstdint>
#include <functional>
#include <limits>
#include <string>
#include <vector>

#include "BackEnd/DeviceTexture.h"
#include "Math/vec2.h"
#include "Rendering/DrawPass.h"

namespace tzw
{
class DeviceFrameBuffer;
class DeviceRenderCommand;
class DeviceRenderPass;
class DeviceRenderStage;
class RenderGraph;
class RenderPath;
class RenderQueue;
struct RenderGraphPassDesc;
struct RenderGraphResource;

class RenderGraphResourceHandle
{
public:
	RenderGraphResourceHandle();

	bool isValid() const;
	uint32_t index() const;
	static RenderGraphResourceHandle invalid();

private:
	friend class RenderGraph;

	explicit RenderGraphResourceHandle(uint32_t index);

	uint32_t m_index;
};

struct RenderGraphResourceDesc
{
	std::string name;
	ImageFormat format = ImageFormat::Surface_Format;
	TextureRoleEnum role = TextureRoleEnum::AS_COLOR;
	TextureUsageEnum usage = TextureUsageEnum::SAMPLE_AND_ATTACHMENT;
	vec2 size = vec2(0, 0);
	bool imported = true;
};

struct RenderGraphResource
{
	RenderGraphResourceDesc desc;
	DeviceTexture* texture = nullptr;
	DeviceTexture* depthTexture = nullptr;
	DeviceFrameBuffer* frameBuffer = nullptr;
};

class RenderGraphContext
{
public:
	RenderGraphContext(DeviceRenderCommand* cmd, RenderPath* renderPath, RenderQueue* sceneQueue);

	DeviceRenderCommand* cmd() const;
	RenderPath* renderPath() const;
	RenderQueue* sceneQueue() const;
	const RenderGraphPassDesc* pass() const;
	DeviceRenderStage* stage() const;
	const RenderGraphResource* resource(RenderGraphResourceHandle handle) const;
	DeviceTexture* texture(RenderGraphResourceHandle handle) const;
	DeviceTexture* depthTexture(RenderGraphResourceHandle handle) const;
	DeviceFrameBuffer* frameBuffer(RenderGraphResourceHandle handle) const;

private:
	friend class RenderGraph;

	void setGraph(const RenderGraph* graph);
	void setPass(const RenderGraphPassDesc* pass);

	DeviceRenderCommand* m_cmd;
	RenderPath* m_renderPath;
	RenderQueue* m_sceneQueue;
	const RenderGraph* m_graph;
	const RenderGraphPassDesc* m_pass;
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
	void clearResources();
	void addPass(const RenderGraphPassDesc& desc);
	RenderGraphResourceHandle importTexture(const RenderGraphResourceDesc& desc, DeviceTexture* texture);
	RenderGraphResourceHandle importFrameBuffer(const RenderGraphResourceDesc& desc, DeviceFrameBuffer* frameBuffer);
	RenderGraphResourceHandle createFrameBuffer(const RenderGraphResourceDesc& desc, DeviceRenderPass* renderPass);
	RenderGraphResource* resource(RenderGraphResourceHandle handle);
	const RenderGraphResource* resource(RenderGraphResourceHandle handle) const;
	DeviceTexture* texture(RenderGraphResourceHandle handle) const;
	DeviceTexture* depthTexture(RenderGraphResourceHandle handle) const;
	DeviceFrameBuffer* frameBuffer(RenderGraphResourceHandle handle) const;
	void execute(RenderGraphContext& context);

private:
	std::vector<RenderGraphPassDesc> m_passes;
	std::vector<RenderGraphResource> m_resources;
};
}
