#pragma once


namespace tzw
{
enum class DeviceBufferType{
	Vertex,
	Index,
	Uniform
};
class DeviceBuffer
{
public:
	DeviceBuffer();
	virtual void allocate(void * data, size_t ammount) = 0;
	virtual void allocateAndSet(size_t alloc_size, void * data, size_t ammount);
	virtual void allocateEmpty(size_t ammount);
	virtual bool init(DeviceBufferType type) = 0;
	unsigned int m_uid;//temporally for opengl
	virtual void bind() = 0;//only for GL
	virtual void setUsePool(bool isUsed) = 0;//only for vulkan
	void setAlignment(size_t newAlignment);
	size_t getAlignment();
	virtual size_t getSize();
	DeviceBufferType m_type {DeviceBufferType::Vertex};
	virtual bool hasEnoughRoom(size_t size);
	virtual bool isValid();
	virtual void map();
	virtual void unmap();
	virtual void copyFrom( void * ptr, size_t size, size_t memOffset = 0);
protected:
	size_t m_alignment = 0;
};
};

