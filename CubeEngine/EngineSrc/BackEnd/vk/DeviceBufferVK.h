#pragma once
#include "../DeviceBuffer.h"
#include "vulkan/vulkan.h"
namespace tzw
{

class DeviceBufferVK : public DeviceBuffer
{
public:
	DeviceBufferVK();
	virtual void allocate(void * data, size_t ammount);
	virtual void allocateEmpty(size_t ammount);
	virtual bool init(DeviceBufferType type);
	virtual void bind();
	VkBuffer getBuffer();
	size_t getAlignment();
	size_t getSize();
	void setAlignment(size_t newAlignment);
	VkDeviceMemory getMemory();
private:
	VkBuffer m_buffer;
	VkDeviceMemory m_memory;
	size_t m_bufferSize;
	size_t m_alignment;
};
};

