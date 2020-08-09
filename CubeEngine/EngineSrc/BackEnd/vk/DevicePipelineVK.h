#pragma once
#include <deque>

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
	void collcetItemWiseDescritporSet();
	VkDescriptorSet giveItemWiseDescriptorSet();
	Material * getMat();
private:
	void createDescriptorPool();
	void createMaterialDescriptorPool();
	void createMaterialUniformBuffer();
	void crateMaterialDescriptorSet();
	void defaultCreateVertexBufferDescription(std::vector<VkVertexInputAttributeDescription> & attributeDescriptions);
	std::deque<VkDescriptorSet> m_itemDescritptorSetList;
	std::vector<VkDescriptorPool> m_descriptorPoolList;
	int m_currItemWiseDescriptorSetIdx;
	size_t m_totalItemWiseDesSet;
	VkDescriptorSet m_materialDescripotrSet;
	Material * m_mat;
	VkPipeline m_pipeline;
	VkPipelineLayout m_pipelineLayout;
	VkBuffer m_matUniformBuffer;
	VkDeviceMemory m_matUniformBufferMemory;
	DeviceShaderVK * m_shader;
	DeviceVertexInput m_vertexInput;
	VkDescriptorPool m_materialDescriptorPool;
};


};

