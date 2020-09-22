#pragma once
#include "Rendering/ImageFormat.h"
#include "vulkan/vulkan.h"
#include <vector>
#include "DeviceRenderPassVK.h"
namespace tzw
{

class DeviceTextureVK;
class DeviceFrameBufferVK
{
public:
	DeviceFrameBufferVK::DeviceFrameBufferVK(int w, int h, VkFramebuffer renderPass);
	DeviceFrameBufferVK(int w, int h, DeviceRenderPassVK * renderPass);
	DeviceTextureVK * getDepthMap();
	std::vector<DeviceTextureVK *> & getTextureList();
	VkFramebuffer getFrameBuffer();
	vec2 getSize();
private:
	VkFramebuffer m_frameBuffer;
	std::vector<DeviceTextureVK *>m_textureList;
	DeviceTextureVK * m_depthTexture;
	vec2 m_size;
};
};

