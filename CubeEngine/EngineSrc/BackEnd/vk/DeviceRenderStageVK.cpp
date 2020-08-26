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
	void DeviceRenderStageVK::setRenderPass(DeviceRenderPassVK* renderPass)
	{
		m_renderPass = renderPass;
	}
	void DeviceRenderStageVK::setFrameBuffer(DeviceFrameBufferVK* frameBuffer)
	{
		m_frameBuffer = frameBuffer;
	}

	void DeviceRenderStageVK::prepare(VkCommandBuffer command, vec4 clearColor, vec2 clearDepthStencil)
	{

		VkCommandBufferBeginInfo beginInfoDeffered = {};
        beginInfoDeffered.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfoDeffered.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

		auto attachmentList = m_renderPass->getAttachmentList();
		std::vector<VkClearValue> clearValuesDefferred{};
		clearValuesDefferred.resize(attachmentList.size());
		for(int i = 0; i < attachmentList.size(); i++)
		{
			if(attachmentList[i].isDepth){
			
				clearValuesDefferred[i].depthStencil = {clearDepthStencil.x, (uint32_t)clearDepthStencil.y};
			}
			else
			{
			
				clearValuesDefferred[i].color = {clearColor.x, clearColor.y, clearColor.z, clearColor.w};
			}
			
		
		}

		VkRenderPassBeginInfo renderPassInfoDeferred = {};
        renderPassInfoDeferred.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfoDeferred.renderPass = getRenderPass()->getRenderPass();   
        renderPassInfoDeferred.renderArea.offset.x = 0;
        renderPassInfoDeferred.renderArea.offset.y = 0;
        renderPassInfoDeferred.renderArea.extent.width = m_frameBuffer->getSize().x;
        renderPassInfoDeferred.renderArea.extent.height = m_frameBuffer->getSize().y;
        renderPassInfoDeferred.clearValueCount = clearValuesDefferred.size();
        renderPassInfoDeferred.pClearValues = clearValuesDefferred.data();

        int res = vkBeginCommandBuffer(command, &beginInfoDeffered);
        CHECK_VULKAN_ERROR("vkBeginCommandBuffer error %d\n", res);
        renderPassInfoDeferred.framebuffer = m_frameBuffer->getFrameBuffer();
        vkCmdBeginRenderPass(command, &renderPassInfoDeferred, VK_SUBPASS_CONTENTS_INLINE);

	}
	void DeviceRenderStageVK::finish(VkCommandBuffer command)
	{
        vkCmdEndRenderPass(command);
        int res = vkEndCommandBuffer(command);
        CHECK_VULKAN_ERROR("vkEndCommandBuffer error %d\n", res);
	}
}