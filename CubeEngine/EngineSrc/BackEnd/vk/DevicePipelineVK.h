#pragma once
#include "vulkan/vulkan.h"
#include <string>
namespace tzw
{
class Material;
class DevicePipelineVK
{
public:
	DevicePipelineVK(Material * mat, VkRenderPass targetRenderPass);
	VkDescriptorSetLayout & getDescriptorSetLayOut();
	VkPipelineLayout & getPipelineLayOut();
	VkPipeline & getPipeline();
private:
	Material * m_mat;
	VkPipeline m_pipeline;
	VkPipelineLayout m_pipelineLayout;
};


};

