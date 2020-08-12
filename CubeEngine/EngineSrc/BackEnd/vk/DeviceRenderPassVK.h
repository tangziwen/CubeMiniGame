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

private:
	VkRenderPass m_renderPass;
	VkFramebuffer m_frameBuffer;
	std::vector<DeviceTextureVK *>m_textureList;
};
};

