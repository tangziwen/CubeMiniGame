#pragma once
#include <deque>
#include "Math/vec2.h"
#include <string>
#include <vector>
#include <functional>
#include "Rendering/RenderFlag.h"
#include "vulkan/vulkan.h"
namespace tzw
{
class Material;
class ShadingParams;
class DeviceRenderPass;
class DeviceDescriptor;
class DeviceShaderCollection;
struct DeviceVertexAttributeDescVK
{
	VkFormat format;
	int offset;

};
struct DeviceVertexInput
{
	int stride;
	void addVertexAttributeDesc(DeviceVertexAttributeDescVK vertexAttributeDesc);
	std::vector<DeviceVertexAttributeDescVK> m_attributeList;

};
const uint32_t PIPELINE_DYNAMIC_STATE_FLAG_NONE = 0;
const uint32_t PIPELINE_DYNAMIC_STATE_FLAG_VIEWPORT = 1 << 1;
const uint32_t PIPELINE_DYNAMIC_STATE_FLAG_SCISSOR = 1 << 2;

class DevicePipeline
{
public:
	virtual ~DevicePipeline() = default;

	DevicePipeline(): m_dynamicState(PIPELINE_DYNAMIC_STATE_FLAG_NONE)
	{
	}
	
	virtual void initCompute(DeviceShaderCollection * computeShader) = 0;
	virtual void init(vec2 viewPortSize, Material * mat, DeviceRenderPass* targetRenderPass
	                  ,DeviceVertexInput vertexInput, bool isSupportInstancing, DeviceVertexInput instanceVertexInput, int colorAttachmentCount = 1) = 0;
	virtual void updateUniform() = 0;
	virtual void updateUniformSingle(std::string name, void * buff, size_t size) = 0;
	virtual DeviceDescriptor * getMaterialDescriptorSet() = 0;
	virtual void resetItemWiseDescritporSet() = 0;
	virtual DeviceDescriptor * giveItemWiseDescriptorSet() = 0;
	void setDynamicState(uint32_t state)
	{
		m_dynamicState = state;
	}
	Material * getMat()
	{
		return m_mat;
	}
	ShadingParams * getShadingParams()
	{
		return m_shadingParams;
	}
	void setShadingParams(ShadingParams * params){ m_shadingParams = params;}
protected:
	uint32_t m_dynamicState = 0;
	Material * m_mat = nullptr;
	ShadingParams * m_shadingParams = nullptr;

};


};

