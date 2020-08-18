#include "DeviceRenderPassVK.h"
#include "../VkRenderBackEnd.h"
#include "DeviceTextureVK.h"
#include "DeviceFrameBufferVK.h"
#include "DeviceRenderStageVK.h"
namespace tzw
{
	DeviceRenderStageVK::DeviceRenderStageVK(DeviceRenderPassVK* renderPass, DeviceFrameBufferVK* frameBuffer)
		:m_renderPass(renderPass),m_frameBuffer(frameBuffer)
	{
	}
	DeviceRenderPassVK* DeviceRenderStageVK::getRenderPass()
	{
		return m_renderPass;
	}
	DeviceFrameBufferVK* DeviceRenderStageVK::getFrameBuffer()
	{
		return m_frameBuffer;
	}
}