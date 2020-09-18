#pragma once
#include "../Rendering/RenderFlag.h"
#include "../Rendering/ImageFormat.h"
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
	virtual DeviceTexture * loadTextureRaw_imp(const unsigned char* buf, int width, int height, ImageFormat format, unsigned int loadingFlag)= 0;
	virtual DeviceShader * createShader_imp() = 0;
	virtual DeviceBuffer * createBuffer_imp() = 0;
	virtual void prepareFrame() = 0;
	virtual void endFrame() = 0;
}; // namespace tzw
}