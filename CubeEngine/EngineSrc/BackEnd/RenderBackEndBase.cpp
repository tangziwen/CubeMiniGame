#include "RenderBackEndBase.h"
namespace tzw
{
	void RenderBackEndBase::initDevice(GLFWwindow* window)
	{
	}
	DeviceTexture* RenderBackEndBase::loadTexture_imp(const unsigned char* buf, size_t buffSize, unsigned int loadingFlag)
	{
		return nullptr;
	}
	DeviceMaterial * RenderBackEndBase::createDeviceMaterial_imp()
	{
		return nullptr;
	}
} // namespace tzw

