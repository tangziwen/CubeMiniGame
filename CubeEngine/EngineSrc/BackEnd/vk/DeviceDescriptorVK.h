#pragma once
#include "vulkan/vulkan.h"
#include "DeviceDescriptorSetLayoutVK.h"
//#include "DeviceItemBufferPoolVK.h"
namespace tzw
{
class DeviceTextureVK;
class DeviceBufferVK;
struct DeviceItemBuffer;
class DeviceDescriptorVK
{
public:
	DeviceDescriptorVK(VkDescriptorSet descSet, DeviceDescriptorSetLayoutVK * layout);
	void updateDescriptorByBinding(int binding, DeviceTextureVK * texture);
	void updateDescriptorByBinding(int binding, std::vector<DeviceTextureVK *>& textureList);
	void updateDescriptorByBinding(int binding, DeviceBufferVK * buffer, size_t offset, size_t range);
	void updateDescriptorByBinding(int binding, DeviceItemBuffer * itemBuff);
	VkDescriptorSet getDescSet();
private:
	VkDescriptorSet m_descriptorSet;
	DeviceDescriptorSetLayoutVK *m_layout;
};
};

