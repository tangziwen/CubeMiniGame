#include "DeviceRenderPassVK.h"
#include "../VkRenderBackEnd.h"
#include "DeviceTextureVK.h"
#include "DeviceFrameBufferVK.h"
#include <array>
#include "Engine/Engine.h"
namespace tzw
{
    DeviceFrameBufferVK::DeviceFrameBufferVK(int w, int h, VkFramebuffer frameBuffer)
    {
        m_size = vec2(w,h);
        m_frameBuffer = frameBuffer;
    }
    DeviceFrameBufferVK::DeviceFrameBufferVK(int w, int h, DeviceRenderPassVK * renderPass)
	{
        m_size = vec2(w,h);
        auto & attachmentList = renderPass->getAttachmentList();
        m_depthTexture = nullptr;
        for(int i =0 ; i < attachmentList.size(); i++){

            if(!attachmentList[i].isDepth)
            {
                auto texture = new DeviceTextureVK();
                texture->initEmpty(w, h, attachmentList[i].format,TextureRtFlagVK::COLOR_ATTACHMENT);
                m_textureList.emplace_back(texture);
            }else
            {
                auto texture = new DeviceTextureVK();
                texture->initEmpty(w, h, ImageFormat::D24_S8, TextureRtFlagVK::DEPTH_ATTACHEMENT);
                m_depthTexture = texture;
                m_textureList.emplace_back(texture);
            }
        }

        std::vector<VkImageView> attachments;
        attachments.resize(attachmentList.size());
        for(int i = 0; i <attachmentList.size(); i++){
            attachments[i] = m_textureList[i]->getImageView();
        }

		VkFramebufferCreateInfo fbufCreateInfo = {};
		fbufCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		fbufCreateInfo.pNext = NULL;
		fbufCreateInfo.renderPass = renderPass->getRenderPass();
		fbufCreateInfo.pAttachments = attachments.data();
		fbufCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		fbufCreateInfo.width = w;
		fbufCreateInfo.height = h;
		fbufCreateInfo.layers = 1;
        
		VK_CHECK_RESULT(vkCreateFramebuffer(VKRenderBackEnd::shared()->getDevice(), &fbufCreateInfo, nullptr, &m_frameBuffer));
	}
    DeviceFrameBufferVK::DeviceFrameBufferVK(DeviceTextureVK* tex, DeviceTextureVK* depth, DeviceRenderPassVK* renderPass)
    {
		std::array<VkImageView, 2> attachments = {
		tex->getImageView(), //color buffer of swap chain
		depth->getImageView(), //the depth buffer
		};
        auto screenSize = Engine::shared()->winSize();
        VkFramebufferCreateInfo fbCreateInfo = {};
        fbCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        fbCreateInfo.renderPass = renderPass->getRenderPass();
        fbCreateInfo.attachmentCount = attachments.size();
        fbCreateInfo.pAttachments = attachments.data();
        fbCreateInfo.width = screenSize.x;
        fbCreateInfo.height = screenSize.y;
        fbCreateInfo.layers = 1;
        m_size = screenSize;
        VK_CHECK_RESULT(vkCreateFramebuffer(VKRenderBackEnd::shared()->getDevice(), &fbCreateInfo, nullptr, &m_frameBuffer));
    }
    DeviceTextureVK* DeviceFrameBufferVK::getDepthMap()
    {
        return m_depthTexture;
    }
    std::vector<DeviceTextureVK*>& DeviceFrameBufferVK::getTextureList()
    {
        return m_textureList;
    }
    VkFramebuffer DeviceFrameBufferVK::getFrameBuffer()
    {
        return m_frameBuffer;
    }
    vec2 DeviceFrameBufferVK::getSize()
    {
        return m_size;
    }
}