#pragma once
#include "Rendering/ImageFormat.h"

namespace tzw
{

class DeviceRenderCommand
{
public:
	DeviceRenderCommand(void * handle);
	virtual void startRecord() = 0;
	virtual void endRecord() = 0;
	void * get();
protected:
	void * m_handle;
};


};

