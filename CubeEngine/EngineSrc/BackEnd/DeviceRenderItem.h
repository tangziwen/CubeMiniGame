#pragma once
#include "Rendering/ImageFormat.h"
#include <vector>
#include "DeviceBuffer.h"
#include "DeviceDescriptor.h"
namespace tzw
{

struct DeviceRenderItem
{
	DeviceDescriptor * m_desc;
	DeviceItemBuffer m_buff;
	void * ptr;
};

};

