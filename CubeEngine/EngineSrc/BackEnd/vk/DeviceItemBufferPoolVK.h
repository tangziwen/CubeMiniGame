#pragma once
#include "../DeviceTexture.h"
#include "vulkan/vulkan.h"
#include <string>

namespace tzw
{
class DeviceItemBufferPoolVK;
struct DeviceItemBuffer
{
	size_t m_offset;
	size_t m_size;
	void map();
	void copyFrom( void * ptr, size_t size, size_t memOffset = 0);
	void unMap();
	void * m_tmpBuff;
	DeviceItemBufferPoolVK * m_pool;
};

class DeviceBufferVK;
class DeviceItemBufferPoolVK
{
public:
	DeviceItemBufferPoolVK(size_t guessSize);
	size_t giveMeBuffer(size_t size);
	DeviceItemBuffer giveMeItemBuffer(size_t size);
	void reset();
	DeviceBufferVK * getBuffer();
private:
	DeviceBufferVK * m_buffer;
	size_t m_currSize;
};


};

