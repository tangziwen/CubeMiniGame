#pragma once
#include "Rendering/ImageFormat.h"
#include "vulkan/vulkan.h"
#include "DeviceRenderPassVK.h"
#include "DeviceFrameBufferVK.h"
#include "Rendering/RenderStage.h"
namespace tzw
{

class DeviceTextureVK;
class DeviceRenderStageVK : public RenderStage
{
public:
	DeviceRenderStageVK(DeviceRenderPassVK * renderPass, DeviceFrameBufferVK * frameBuffer);
	DeviceRenderPassVK * getRenderPass();
	DeviceFrameBufferVK * getFrameBuffer();
	void setRenderPass(DeviceRenderPassVK * renderPass);
	void setFrameBuffer(DeviceFrameBufferVK * frameBuffer);
	void prepare(vec4 clearColor = vec4(0, 0, 0, 1), vec2 clearDepthStencil = vec2(1, 0));
	void finish();
	void draw(std::vector<RenderCommand> & cmdList);
	void drawScreenQuad();
	void drawSphere();
	VkCommandBuffer getCommand();
	void createSinglePipeline(Material * material);
	DevicePipelineVK * getSinglePipeline();
	void bindSinglePipelineDescriptor();
	void bindSinglePipelineDescriptor(DeviceDescriptorVK * extraItemDescriptor);
private:
	void initFullScreenQuad();
	void initSphere();
	void fetchCommand();
	DeviceRenderPassVK * m_renderPass;
	DeviceFrameBufferVK * m_frameBuffer;
	DevicePipelineVK * m_singlePipeline;
	VkCommandBuffer m_command;
	std::unordered_map<Material *, DevicePipelineVK *>m_matPipelinePool;
	std::unordered_set<DevicePipelineVK *> m_fuckingObjList;
    static DeviceBufferVK * m_quadVertexBuffer;
    static DeviceBufferVK * m_quadIndexBuffer;
	static Mesh * m_sphere;
};
};

