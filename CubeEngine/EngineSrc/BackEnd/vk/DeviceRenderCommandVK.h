#pragma once
#include "../DeviceRenderCommand.h"
#include "vulkan/vulkan.h"
namespace tzw
{

class DeviceRenderCommandVK : public DeviceRenderCommand
{
public:
	DeviceRenderCommandVK(VkCommandBuffer handle);
	void startRecord() override;
	void endRecord() override;

	VkCommandBuffer getVK()
	{
		return static_cast<VkCommandBuffer_T*>(m_handle);
	}
};


};

