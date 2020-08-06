#pragma once
#include "vulkan/vulkan.h"
#include <string>
#include <vector>
#include <functional>
namespace tzw
{
class Material;
class DeviceShaderVK;
struct DeviceVertexAttributeDescVK
{
	VkFormat format;
	int offset;

};
struct DeviceVertexInput{
	int stride;
	void addVertexAttributeDesc(DeviceVertexAttributeDescVK vertexAttributeDesc);
	std::vector<DeviceVertexAttributeDescVK> m_attributeList;

};
class DevicePipelineVK
{
public:
	DevicePipelineVK(Material * mat, VkRenderPass targetRenderPass
		,DeviceVertexInput vertexInput);
	VkDescriptorSetLayout getDescriptorSetLayOut();
	VkPipelineLayout getPipelineLayOut();
	VkPipeline getPipeline();
	VkDescriptorSet getMaterialDescriptorSet();
	void updateMaterialDescriptorSet();
	void updateUniform();
private:
	void createMaterialUniformBuffer();
	void crateMaterialDescriptorSet();
	void defaultCreateVertexBufferDescription(std::vector<VkVertexInputAttributeDescription> & attributeDescriptions);
	VkDescriptorSet m_materialDescripotrSet;
	Material * m_mat;
	VkPipeline m_pipeline;
	VkPipelineLayout m_pipelineLayout;
	VkBuffer m_matUniformBuffer;
	VkDeviceMemory m_matUniformBufferMemory;
	DeviceShaderVK * m_shader;
	DeviceVertexInput m_vertexInput;
};


};

