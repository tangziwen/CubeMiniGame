#pragma once
#include "Rendering/ImageFormat.h"
#include "vulkan/vulkan.h"
#include <vector>
#include "DeviceRenderPassVK.h"
#include "Math/vec2.h"
#include "../DeviceFrameBuffer.h"
namespace tzw
{

class DeviceTextureVK;
class DeviceFrameBufferVK : public DeviceFrameBuffer
{
public:
	DeviceFrameBufferVK() = default;
	void init(int w, int h, VkFramebuffer renderPass);
	void init(int w, int h, DeviceRenderPass * renderPass) override;
	void init(DeviceTexture * tex, DeviceTexture * depth, DeviceRenderPass * renderPass) override;
	DeviceTexture * getDepthMap() override;
	std::vector<DeviceTexture *> & getTextureList() override;
	vec2 getSize() override;
	VkFramebuffer getFrameBuffer();
private:
	VkFramebuffer m_frameBuffer;
};
};

