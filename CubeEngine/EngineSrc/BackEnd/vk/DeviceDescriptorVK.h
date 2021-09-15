#pragma once
#include "vulkan/vulkan.h"
#include "DeviceDescriptorSetLayoutVK.h"
#include "../DeviceDescriptor.h"
//#include "DeviceItemBufferPoolVK.h"
namespace tzw
{
class DeviceTextureVK;
class DeviceBufferVK;
struct DeviceItemBuffer;
class DeviceDescriptorVK : public DeviceDescriptor
{
public:
	DeviceDescriptorVK(VkDescriptorSet descSet, DeviceDescriptorSetLayoutVK * layout);
	void updateDescriptorByBinding(int binding, DeviceTexture * texture) override;
	void updateDescriptorByBinding(int binding, std::vector<DeviceTexture *>& textureList) override;
	void updateDescriptorByBinding(int binding, DeviceBuffer * buffer, size_t offset, size_t range) override;
	void updateDescriptorByBinding(int binding, DeviceItemBuffer * itemBuff) override;
	void updateDescriptorByBindingAsStorageImage(int binding, DeviceTexture * texture) override;
	VkDescriptorSet getDescSet();
private:
	VkDescriptorSet m_descriptorSet;
	DeviceDescriptorSetLayoutVK *m_layout;
};
};

