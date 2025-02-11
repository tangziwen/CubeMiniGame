#pragma once
#include "Rendering/ImageFormat.h"
#include "Rendering/RenderStage.h"
#include "Rendering/RenderCommand.h"
#include "DeviceRenderPass.h"
#include <unordered_map>
#include <unordered_set>
#include "DeviceFrameBuffer.h"
#include "DevicePipeline.h"
#include "DeviceMaterial.h"
#include "DeviceBuffer.h"
#include "DeviceDescriptor.h"
#include "DeviceRenderCommand.h"
#include "Rendering/RenderQueues.h"
namespace tzw
{
class DeviceRenderStage
{
public:
	DeviceRenderStage();
	void init(DeviceRenderPass * renderPass, DeviceFrameBuffer * frameBuffer, uint32_t renderStage = (uint32_t)RenderFlag::RenderStage::All);
	void initCompute();
	DeviceRenderPass * getRenderPass();
	DeviceFrameBuffer * getFrameBuffer();
	void setRenderPass(DeviceRenderPass * renderPass);
	void setFrameBuffer(DeviceFrameBuffer * frameBuffer);
	virtual void prepare(DeviceRenderCommand* renderCommand);
	virtual void finish() = 0;
	virtual void draw(RenderQueue * renderQueue) = 0;
	virtual void drawScreenQuad() = 0;
	virtual void drawSphere() = 0;
	virtual void dispatch(unsigned int x, unsigned int y, unsigned int z) = 0;
	virtual void beginCompute() = 0;
	virtual void endCompute() = 0;
	void createSinglePipeline(Material * material);
	void createSingleComputePipeline(DeviceShaderCollection * shaderCollection);
	DevicePipeline * getSinglePipeline();
	DeviceMaterial * getSolorDeviceMaterial();
	virtual void bindSinglePipelineDescriptor() = 0;
	virtual void bindSinglePipelineDescriptor(DeviceDescriptor * extraItemDescriptor) = 0;
	virtual void bindSinglePipelineDescriptorCompute() = 0;
	virtual void bindPipeline(DevicePipeline * pipeline) = 0;
	virtual void bindDescriptor(DevicePipeline * pipeline, std::vector<DeviceDescriptor *> descriptorList) = 0;
	virtual void beginRenderPass(DeviceFrameBuffer* buffer = nullptr, vec4 clearColor = vec4(0, 0, 0, 1), vec2 clearDepthStencil = vec2(1, 0)) = 0;
	virtual void endRenderPass() = 0;
	virtual void bindVBO(DeviceBuffer * buf) = 0;
	virtual void bindIBO(DeviceBuffer * buf) = 0;
	virtual void setScissor(vec4 scissorRect) = 0;
	virtual void drawElement(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance) = 0;
	std::string & getName(){m_name;}
	void setName(std::string newName){m_name = newName;}
	RenderQueue * getSelfRenderQueue();
protected:
	void initFullScreenQuad();
	void initFullScreenTriangleOptimized();
	void initSphere();
	virtual void fetchCommand() = 0;
	DeviceRenderPass * m_renderPass;
	DeviceFrameBuffer * m_frameBuffer;
	DevicePipeline * m_singlePipeline;
	DeviceMaterial * m_soloMaterial;
	std::unordered_map<Material *, DevicePipeline *>m_matPipelinePool;
	std::unordered_map<Material *, DeviceMaterial *>m_deviceMaterialPool;
	std::unordered_map<std::string, DevicePipeline *>m_pipelinePool;
	std::unordered_set<DeviceMaterial *> m_activeMatList;
    static DeviceBuffer * m_quadVertexBuffer;
    static DeviceBuffer * m_quadIndexBuffer;
	static Mesh * m_sphere;
	std::string m_name;
	DeviceRenderCommand * m_deviceRenderCommand;
	RenderQueue * m_selfRenderQueue;
	uint32_t m_renderStage;
};
};

