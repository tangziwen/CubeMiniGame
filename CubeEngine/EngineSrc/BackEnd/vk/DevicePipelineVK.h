#pragma once
#include <deque>

#include "vulkan/vulkan.h"
#include <string>
#include <vector>
#include <functional>
#include "DeviceDescriptorVK.h"
namespace tzw
{
class Material;
class DeviceShaderVK;
class DeviceTextureVK;
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
		,DeviceVertexInput vertexInput, bool isSupportInstancing, DeviceVertexInput instanceVertexInput, int colorAttachmentCount = 1);
	VkDescriptorSetLayout getDescriptorSetLayOut();
	VkPipelineLayout getPipelineLayOut();
	VkPipeline getPipeline();
	VkDescriptorSet getMaterialDescriptorSet();
	void updateMaterialDescriptorSet();
	void updateUniform();
	void collcetItemWiseDescritporSet();
	DeviceDescriptorVK * giveItemWiseDescriptorSet();
	Material * getMat();
	void updateDescriptorByBinding(VkDescriptorSet descSet, int binding, DeviceTextureVK * texture);
private:
	void createDescriptorPool();
	void createMaterialDescriptorPool();
	void createMaterialUniformBuffer();
	void crateMaterialDescriptorSet();
	void defaultCreateVertexBufferDescription(std::vector<VkVertexInputAttributeDescription> & attributeDescriptions);
	std::deque<DeviceDescriptorVK *> m_itemDescritptorSetList;
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

