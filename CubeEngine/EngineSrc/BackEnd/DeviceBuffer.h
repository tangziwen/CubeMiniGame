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
	virtual bool init(DeviceBufferType type) = 0;
	unsigned int m_uid;//temporally for opengl
	virtual void bind() = 0;//only for GL
	DeviceBufferType m_type;
};
};

