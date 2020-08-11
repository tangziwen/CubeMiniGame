#pragma once
#include "../DeviceBuffer.h"

namespace tzw
{

class DeviceBufferGL : public DeviceBuffer
{
public:
	virtual void allocate(void * data, size_t ammount);
	virtual bool init(DeviceBufferType type);
	virtual void bind();//only for GL
	void setUsePool(bool isUsed) override;
};
};

