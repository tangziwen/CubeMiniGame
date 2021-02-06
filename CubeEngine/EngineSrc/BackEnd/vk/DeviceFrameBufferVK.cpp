#include "DeviceRenderPassVK.h"
#include "../VkRenderBackEnd.h"
#include "DeviceTextureVK.h"
#include "DeviceFrameBufferVK.h"
#include <array>
#include "Engine/Engine.h"
namespace tzw
{
    void DeviceFrameBufferVK::init(int w, int h, VkFramebuffer frameBuffer)
    {
        m_size = vec2(w,h);
        m_frameBuffer = frameBuffer;
    }
    void DeviceFrameBufferVK::init(int w, int h, DeviceRenderPass * renderPass)
	{
        m_size = vec2(w,h);
        auto & attachmentList = renderPass->getAttachmentList();
        m_depthTexture = nullptr;
        for(int i =0 ; i < attachmentList.size(); i++){

            if(!attachmentList[i].isDepth)
            {
                auto texture = new DeviceTextureVK();
                texture->initEmpty(w, h, attachmentList[i].format,TextureRoleEnum::AS_COLOR, TextureUsageEnum::SAMPLE_AND_ATTACHMENT);
                m_textureList.emplace_back(texture);
            }else
            {
                auto texture = new DeviceTextureVK();
                texture->initEmpty(w, h, ImageFormat::D24_S8, TextureRoleEnum::AS_DEPTH, TextureUsageEnum::SAMPLE_AND_ATTACHMENT);
                m_depthTexture = texture;
                m_textureList.emplace_back(texture);
            }
        }

        std::vector<VkImageView> attachments;
        attachments.resize(attachmentList.size());
        for(int i = 0; i <attachmentList.size(); i++){
            attachments[i] = static_cast<DeviceTextureVK *>(m_textureList[i])->getImageView();
        }

		VkFramebufferCreateInfo fbufCreateInfo = {};
		fbufCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		fbufCreateInfo.pNext = NULL;
		fbufCreateInfo.renderPass = static_cast<DeviceRenderPassVK *>(renderPass)->getRenderPass();
		fbufCreateInfo.pAttachments = attachments.data();
		fbufCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		fbufCreateInfo.width = w;
		fbufCreateInfo.height = h;
		fbufCreateInfo.layers = 1;
        
		VK_CHECK_RESULT(vkCreateFramebuffer(VKRenderBackEnd::shared()->getDevice(), &fbufCreateInfo, nullptr, &m_frameBuffer));
	}
    void DeviceFrameBufferVK::init(DeviceTexture* tex, DeviceTexture* depth, DeviceRenderPass* renderPass)
    {
		std::array<VkImageView, 2> attachments = {
		static_cast<DeviceTextureVK *>(tex)->getImageView(), //color buffer of swap chain
		static_cast<DeviceTextureVK *>(depth)->getImageView(), //the depth buffer
		};
        auto screenSize = Engine::shared()->winSize();
        VkFramebufferCreateInfo fbCreateInfo = {};
        fbCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        fbCreateInfo.renderPass = static_cast<DeviceRenderPassVK *>(renderPass)->getRenderPass();
        fbCreateInfo.attachmentCount = attachments.size();
        fbCreateInfo.pAttachments = attachments.data();
        fbCreateInfo.width = screenSize.x;
        fbCreateInfo.height = screenSize.y;
        fbCreateInfo.layers = 1;
        m_size = screenSize;
        VK_CHECK_RESULT(vkCreateFramebuffer(VKRenderBackEnd::shared()->getDevice(), &fbCreateInfo, nullptr, &m_frameBuffer));
    }
    DeviceTexture* DeviceFrameBufferVK::getDepthMap()
    {
        return m_depthTexture;
    }
    std::vector<DeviceTexture*>& DeviceFrameBufferVK::getTextureList()
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