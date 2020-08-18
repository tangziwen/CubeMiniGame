#pragma once
#include "Rendering/ImageFormat.h"
#include "vulkan/vulkan.h"
#include "DeviceRenderPassVK.h"
#include "DeviceFrameBufferVK.h"
namespace tzw
{

class DeviceTextureVK;
class DeviceRenderStageVK
{
public:
	DeviceRenderStageVK(DeviceRenderPassVK * renderPass, DeviceFrameBufferVK * frameBuffer);
	DeviceRenderPassVK * getRenderPass();
	DeviceFrameBufferVK * getFrameBuffer();
private:
	DeviceRenderPassVK * m_renderPass;
	DeviceFrameBufferVK * m_frameBuffer;
};
};

