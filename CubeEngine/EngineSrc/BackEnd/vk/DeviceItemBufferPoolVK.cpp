#include "DeviceItemBufferPoolVK.h"
#include "DeviceBufferVK.h"
namespace tzw
{



	DeviceItemBufferPoolVK::DeviceItemBufferPoolVK(size_t guessSize)
	{
		m_buffer = new DeviceBufferVK();
		m_buffer->init(DeviceBufferType::Uniform);
		m_buffer->allocateEmpty(guessSize);
		m_currSize = 0;
	}

	size_t DeviceItemBufferPoolVK::giveMeBuffer(size_t size)
	{
		size_t currBuff = m_currSize;
		m_currSize += size;
		return currBuff;
	}

	void DeviceItemBufferPoolVK::reset()
	{
		m_currSize = 0;
	}

	DeviceBufferVK* DeviceItemBufferPoolVK::getBuffer()
	{
		return m_buffer;
	}

}
