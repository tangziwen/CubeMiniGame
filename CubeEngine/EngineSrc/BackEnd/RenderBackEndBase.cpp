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
} // namespace tzw

