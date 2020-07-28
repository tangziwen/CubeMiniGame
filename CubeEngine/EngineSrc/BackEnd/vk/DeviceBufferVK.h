#pragma once
#include "../DeviceBuffer.h"

namespace tzw
{

class DeviceBufferVK : public DeviceBuffer
{
public:
	virtual void allocate(void * data, size_t ammount);
	virtual bool init(DeviceBufferType type);
	virtual void bind();
};
};

