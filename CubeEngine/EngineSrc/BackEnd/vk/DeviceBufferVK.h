#pragma once
#include "../DeviceBuffer.h"
#include "vulkan/vulkan.h"
namespace tzw
{

class DeviceBufferVK : public DeviceBuffer
{
public:
	virtual void allocate(void * data, size_t ammount);
	virtual bool init(DeviceBufferType type);
	virtual void bind();
	VkBuffer & getBuffer();
private:
	VkBuffer m_buffer;
};
};

