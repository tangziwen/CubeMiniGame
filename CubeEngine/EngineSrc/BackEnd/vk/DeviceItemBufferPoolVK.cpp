#include "DeviceItemBufferPoolVK.h"
#include "DeviceBufferVK.h"
#include "../VkRenderBackEnd.h"
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
		auto deviceProperties = VKRenderBackEnd::shared()->GetPhysDeviceProperties();
		size_t minAlignment = deviceProperties.limits.minUniformBufferOffsetAlignment;
		size_t currBuff = m_currSize;
		m_currSize += ((size - 1) / minAlignment  + 1) * minAlignment;
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
