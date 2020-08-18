#pragma once
#include "Rendering/ImageFormat.h"
#include "vulkan/vulkan.h"
#include <vector>
namespace tzw
{

class DeviceTextureVK;
class DeviceRenderPassVK
{
public:
	DeviceRenderPassVK(int width, int height, int colorAttachNum, ImageFormat format);
	VkRenderPass getRenderPass();
	VkFramebuffer getFrameBuffer();
	DeviceTextureVK * getDepthMap();
	std::vector<DeviceTextureVK *> & getTextureList();
private:
	VkRenderPass m_renderPass;
	VkFramebuffer m_frameBuffer;
	std::vector<DeviceTextureVK *>m_textureList;
	DeviceTextureVK * m_depthTexture;
};
};

