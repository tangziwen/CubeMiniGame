#pragma once
#include "Rendering/ImageFormat.h"
#include "vulkan/vulkan.h"
#include "DeviceRenderPassVK.h"
#include "DeviceFrameBufferVK.h"
#include "Rendering/RenderStage.h"
#include "Rendering/RenderCommand.h"
#include <unordered_map>
#include <unordered_set>
#include "../DeviceRenderStage.h"
namespace tzw
{

class DeviceTextureVK;
class DeviceDescriptorVK;
class DeviceRenderStageVK : public DeviceRenderStage
{
public:
	DeviceRenderStageVK();
	void finish();
	void draw(RenderQueue * renderQueue) override;
	void drawScreenQuad();
	void drawSphere();
	void bindSinglePipelineDescriptor() override;
	void bindSinglePipelineDescriptor(DeviceDescriptor * extraItemDescriptor) override;
	void bindPipeline(DevicePipeline * pipeline) override;
	void bindDescriptor(DevicePipeline * pipeline, std::vector<DeviceDescriptor *> descriptorList) override;
	void beginRenderPass(vec4 clearColor = vec4(0, 0, 0, 1), vec2 clearDepthStencil = vec2(1, 0));
	void endRenderPass();
	void bindVBO(DeviceBuffer * buf) override;
	void bindIBO(DeviceBuffer * buf) override;
	void setScissor(vec4 scissorRect);
	void drawElement(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance);
protected:
	void fetchCommand() override;
};
};

