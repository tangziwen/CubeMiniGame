#pragma once
#include <deque>
#include "Math/vec2.h"
#include "vulkan/vulkan.h"
#include <string>
#include <vector>
#include <functional>
#include "DeviceDescriptorVK.h"
#include "Rendering/RenderFlag.h"
#include "../DevicePipeline.h"
#include "../DeviceRenderItem.h"
namespace tzw
{
class Material;
class DeviceShaderCollectionVK;
class DeviceTextureVK;
class DeviceRenderPassVK;

class DevicePipelineVK : public DevicePipeline
{
public:
	DevicePipelineVK();
	void initCompute(DeviceShaderCollection * computeShader) override;
	void init(vec2 viewPortSize, Material * mat, DeviceRenderPass* targetRenderPass
		,DeviceVertexInput vertexInput, bool isSupportInstancing, DeviceVertexInput instanceVertexInput, int colorAttachmentCount = 1) override;
	VkDescriptorSetLayout getDescriptorSetLayOut();
	VkDescriptorSetLayout getMaterialDescriptorSetLayOut();
	VkPipelineLayout getPipelineLayOut();
	VkPipeline getPipeline();
	void resetItemWiseDescritporSet() override;
	DeviceDescriptor * giveItemWiseDescriptorSet() override;
	DeviceRenderItem * getRenderItem(void * obj);
private:
	void createDescriptorPool();
	std::deque<DeviceDescriptorVK *> m_itemDescritptorSetList;
	std::vector<VkDescriptorPool> m_descriptorPoolList;
	int m_currItemWiseDescriptorSetIdx;
	size_t m_totalItemWiseDesSet;
	VkPipeline m_pipeline;
	VkPipelineLayout m_pipelineLayout;
	DeviceShaderCollectionVK * m_shader;
	DeviceVertexInput m_vertexInput;
	VkBlendFactor getBlendFactor(RenderFlag::BlendingFactor factor);
	std::unordered_map<void *, DeviceRenderItem *> m_renderItemMap;
	DeviceItemBufferPoolVK * m_itemBufferPool;
};


};

