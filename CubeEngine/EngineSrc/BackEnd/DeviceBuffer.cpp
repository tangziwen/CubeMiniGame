#include "DeviceBuffer.h"

tzw::DeviceBuffer::DeviceBuffer()
{
	m_uid = 0;
}

void tzw::DeviceBuffer::allocateEmpty(size_t ammount)
{
}

void tzw::DeviceBuffer::setAlignment(size_t newAlignment)
{
	m_alignment = newAlignment;
}

size_t tzw::DeviceBuffer::getAlignment()
{
	return m_alignment;
}

size_t tzw::DeviceBuffer::getSize()
{
	return 0;
}

bool tzw::DeviceBuffer::hasEnoughRoom(size_t size)
{
	return false;
}

bool tzw::DeviceBuffer::isValid()
{
	return true;
}

void tzw::DeviceBuffer::map()
{
}

void tzw::DeviceBuffer::unmap()
{
}

void tzw::DeviceBuffer::copyFrom(void* ptr, size_t size, size_t memOffset)
{
}
