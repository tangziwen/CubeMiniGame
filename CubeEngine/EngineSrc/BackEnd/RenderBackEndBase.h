#pragma once
#include "../Rendering/RenderFlag.h"
#include <string>
class GLFWwindow;

namespace tzw {
class DeviceTexture;
class DeviceShader;
class DeviceBuffer;
class RenderBackEndBase
{
public:
	virtual void initDevice(GLFWwindow * window);
	virtual DeviceTexture * loadTexture_imp(const unsigned char* buf, size_t buffSize, unsigned int loadingFlag);
	virtual DeviceShader * createShader_imp() = 0;
	virtual DeviceBuffer * createBuffer_imp() = 0;
}; // namespace tzw
}