#pragma once
#include "../Rendering/RenderFlag.h"
#include <string>
class GLFWwindow;

namespace tzw {
class DeviceTexture;
class RenderBackEndBase
{
public:
	virtual void initDevice(GLFWwindow * window);
	virtual DeviceTexture * loadTexture_imp(const unsigned char* buf, size_t buffSize, unsigned int loadingFlag);
}; // namespace tzw
}