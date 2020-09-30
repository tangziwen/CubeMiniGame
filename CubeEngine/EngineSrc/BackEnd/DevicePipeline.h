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
class DeviceRenderPass;
class DeviceDescriptor;
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
class DevicePipeline
{
public:
	DevicePipeline() = default;
	virtual void init(vec2 viewPortSize, Material * mat, DeviceRenderPass* targetRenderPass
		,DeviceVertexInput vertexInput, bool isSupportInstancing, DeviceVertexInput instanceVertexInput, int colorAttachmentCount = 1) = 0;
	virtual void updateUniformSingle(std::string name, void * buff, size_t size) = 0;
	virtual DeviceDescriptor * getMaterialDescriptorSet() = 0;
	virtual void collcetItemWiseDescritporSet() = 0;
	virtual DeviceDescriptor * giveItemWiseDescriptorSet() = 0;
};


};

