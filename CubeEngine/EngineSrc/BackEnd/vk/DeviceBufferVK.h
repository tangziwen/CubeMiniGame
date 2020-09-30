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
	bool hasEnoughRoom(size_t size) override;
	bool isValid() override;
	VkBuffer getBuffer();
	virtual size_t getSize();
	size_t getOffset();
	VkDeviceMemory getMemory();
	void setUsePool(bool isUsed) override;
	void map() override;
	void unmap() override;
	void copyFrom( void * ptr, size_t size, size_t memOffset = 0) override;
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

