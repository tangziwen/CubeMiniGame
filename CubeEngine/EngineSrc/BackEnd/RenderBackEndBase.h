#pragma once
#include "../Rendering/RenderFlag.h"
#include "../Rendering/ImageFormat.h"
#include <string>
class GLFWwindow;

namespace tzw {
class DeviceTexture;
class DeviceRenderCommand;
class DeviceShaderCollection;
class DeviceBuffer;
class RenderPath;
class DeviceRenderPass;
class DeviceRenderStage;
class DeviceFrameBuffer;
class DevicePipeline;
class DeviceMaterial;
class RenderBackEndBase
{
public:
	virtual void initDevice(GLFWwindow * window);
	virtual DeviceTexture * loadTexture_imp(const unsigned char* buf, size_t buffSize, unsigned int loadingFlag);
	virtual DeviceTexture * loadTextureRaw_imp(const unsigned char* buf, int width, int height, ImageFormat format, unsigned int loadingFlag)= 0;
	virtual DeviceShaderCollection * createShader_imp() = 0;
	virtual DeviceBuffer * createBuffer_imp() = 0;
	virtual DeviceRenderPass * createDeviceRenderpass_imp() = 0;
	virtual DevicePipeline * createPipeline_imp() = 0;
	virtual DeviceRenderStage * createRenderStage_imp() = 0;
	virtual DeviceFrameBuffer * createFrameBuffer_imp() = 0;
	virtual DeviceMaterial * createDeviceMaterial_imp();
	virtual DeviceRenderCommand* getGeneralCommandBuffer() { return nullptr; }
	virtual DeviceFrameBuffer* getSwapChainFrameBuffer(unsigned int imageIndex) { return nullptr; }
	virtual unsigned int getSwapChainImageCount() const { return 0; }
	virtual int getCurrSwapIndex() { return 0; }
	virtual void prepareFrame() = 0;
	virtual void endFrame(RenderPath * renderPath) = 0;
}; // namespace tzw
}