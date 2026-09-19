#pragma once
#include "../DeviceBuffer.h"
#include <string>

namespace tzw
{
class DeviceBufferVK;
class DeviceItemBufferPoolVK
{
public:
	DeviceItemBufferPoolVK(size_t guessSize);
	~DeviceItemBufferPoolVK();
	size_t giveMeBuffer(size_t size);
	DeviceItemBuffer giveMeItemBuffer(size_t size);
	void reset();
	DeviceBufferVK * getBuffer();
private:
	DeviceBufferVK * m_buffer;
	size_t m_currSize;
};


};

