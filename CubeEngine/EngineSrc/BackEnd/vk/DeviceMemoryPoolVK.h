#pragma once

#include "vulkan/vulkan.h"

namespace tzw
{

struct BufferInfo
{
	
	size_t getSize();
	size_t getOffset();
	VkDeviceMemory getMemory();
	void * m_ptr;	
};
class DeviceMemoryPoolVK
{
public:
	DeviceMemoryPoolVK(VkPhysicalDevice phyDevice, VkDevice device, VkInstance inst);
	BufferInfo getBuffer(VkBufferCreateInfo bufferCreateInfo, VkBuffer* targetBuffer);
	void destroyBuffer(VkBuffer targetBuffer, BufferInfo info);
	void map(BufferInfo info, void ** data);
	void unmap(BufferInfo info);
};
};

