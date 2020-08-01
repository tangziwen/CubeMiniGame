#pragma once
#include "vulkan/vulkan.h"
#include <string>
namespace tzw
{
class Material;
class DeviceShaderVK;
class DevicePipelineVK
{
public:
	DevicePipelineVK(Material * mat, VkRenderPass targetRenderPass);
	VkDescriptorSetLayout & getDescriptorSetLayOut();
	VkPipelineLayout & getPipelineLayOut();
	VkPipeline & getPipeline();
	VkDescriptorSet & getMaterialDescriptorSet();
	void updateMaterialDescriptorSet();
	void updateUniform();
private:
	void createMaterialUniformBuffer();
	void crateMaterialDescriptorSet();
	VkDescriptorSet m_materialDescripotrSet;
	Material * m_mat;
	VkPipeline m_pipeline;
	VkPipelineLayout m_pipelineLayout;
	VkBuffer m_matUniformBuffer;
	VkDeviceMemory m_matUniformBufferMemory;
	DeviceShaderVK * m_shader;
};


};

