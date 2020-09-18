#include "DeviceItemBufferPoolVK.h"
#include "DeviceBufferVK.h"
#include "../VkRenderBackEnd.h"
namespace tzw
{


	void DeviceItemBuffer::map()
	{
        vkMapMemory(VKRenderBackEnd::shared()->getDevice(), m_pool->getBuffer()->getMemory(), m_offset, m_size, 0, &m_tmpBuff);
	}

	void DeviceItemBuffer::copyFrom(void* ptr, size_t size, size_t memOffset)
	{
		memcpy((char *)m_tmpBuff + memOffset, ptr, size);
	}

	void DeviceItemBuffer::unMap()
	{
		vkUnmapMemory(VKRenderBackEnd::shared()->getDevice(), m_pool->getBuffer()->getMemory());
	}

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

	DeviceItemBuffer DeviceItemBufferPoolVK::giveMeItemBuffer(size_t size)
	{
		DeviceItemBuffer buf;
		size_t mem_offset = giveMeBuffer(size);
		buf.m_offset = mem_offset;
		buf.m_size = size;
		buf.m_pool = this;
		return buf;
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
