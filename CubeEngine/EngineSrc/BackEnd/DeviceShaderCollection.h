#pragma once
#include "DeviceShader.h"

namespace tzw
{

class DeviceShaderCollection
{
public:
	virtual void addShader(const unsigned char * buff, size_t size, DeviceShaderType type, const unsigned char * fileInfoStr) = 0;
	virtual bool create() = 0;
	virtual bool finish() = 0;
	unsigned int m_uid;//temporally for opengl

};
};

