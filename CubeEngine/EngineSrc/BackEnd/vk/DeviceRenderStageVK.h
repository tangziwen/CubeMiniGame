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
	void drawScreenQuad() override;
	void drawSphere() override;
	void dispatch(unsigned int x, unsigned int y, unsigned int z) override;
	void bindSinglePipelineDescriptor() override;
	void bindSinglePipelineDescriptor(DeviceDescriptor * extraItemDescriptor) override;
	virtual void bindSinglePipelineDescriptorCompute() override;
	void bindPipeline(DevicePipeline * pipeline) override;
	void bindDescriptor(DevicePipeline * pipeline, std::vector<DeviceDescriptor *> descriptorList) override;
	void beginRenderPass(DeviceFrameBuffer* buffer = nullptr,vec4 clearColor = vec4(0, 0, 0, 1), vec2 clearDepthStencil = vec2(1, 0)) override;
	void endRenderPass();
	void beginCompute() override;
	void endCompute() override;
	void bindVBO(DeviceBuffer * buf) override;
	void bindIBO(DeviceBuffer * buf) override;
	void setScissor(vec4 scissorRect);
	void drawElement(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance);
protected:
	void fetchCommand() override;
};
};

