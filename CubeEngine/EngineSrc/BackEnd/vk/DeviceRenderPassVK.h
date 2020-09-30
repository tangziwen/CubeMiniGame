#pragma once
#include "Rendering/ImageFormat.h"
#include "vulkan/vulkan.h"
#include <vector>
#include "../DeviceRenderPass.h"
namespace tzw
{

class DeviceTextureVK;
class DeviceRenderPassVK : public DeviceRenderPass
{
public:
	DeviceRenderPassVK();
	void init(int colorAttachNum, OpType opType, ImageFormat format,bool isNeedTransitionToRread, bool isOutputToScreen = false) override;
	VkRenderPass getRenderPass();
private:
	VkRenderPass m_renderPass;
};
};

