#pragma once
#include "../DeviceBuffer.h"
#include "vulkan/vulkan.h"
#include "DeviceMemoryPoolVK.h"
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
	size_t getOffset();
	void setAlignment(size_t newAlignment);
	VkDeviceMemory getMemory();
	void setUsePool(bool isUsed) override;
	void map();
	void unmap();
	void copyFrom( void * ptr, size_t size, size_t memOffset = 0);
private:
	void allocateEmptySingleImp(size_t ammount);
	void allocateEmptyPoolImp(size_t ammount);
	VkBuffer m_buffer;
	VkDeviceMemory m_memory;
	size_t m_bufferSize;
	size_t m_alignment;
	size_t m_offset;
	bool m_isUsePool;
	void * m_tmpData;
	BufferInfo m_bufferInfo;// only used for pool allocation
};
};

