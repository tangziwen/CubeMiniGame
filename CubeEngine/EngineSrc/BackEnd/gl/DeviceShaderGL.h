#pragma once

#include "../DeviceShader.h"
namespace tzw
{

class DeviceShaderGL : public DeviceShader
{
public:
	void addShader(const unsigned char * buff, size_t size, DeviceShaderType type, const unsigned char * fileInfoStr) override;
	bool create() override;
	bool finish() override;
};
};

