#include "DeviceRenderPassVK.h"
#include "../VkRenderBackEnd.h"
#include "DeviceTextureVK.h"
namespace tzw
{
	DeviceRenderPassVK::DeviceRenderPassVK(int width, int height, int colorAttachNum, ImageFormat format)
	{
        std::vector<VkAttachmentReference> colorAttachmentRefs;
        std::vector<VkAttachmentDescription> attachmentDescList;
        for(int i =0 ; i < colorAttachNum; i++){
            VkAttachmentReference attachRef = {};
            attachRef.attachment = i;
            attachRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            colorAttachmentRefs.emplace_back(attachRef);

            auto texture = new DeviceTextureVK();
            texture->initEmpty(width, height, format,TextureRtFlagVK::COLOR_ATTACHMENT);
            m_textureList.emplace_back(texture);
            VkAttachmentDescription attachDesc = {};    
            attachDesc.format = VKRenderBackEnd::shared()->getFormat(format);
            attachDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            attachDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            attachDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            attachDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            attachDesc.initialLayout = texture->getImageLayOut();
            attachDesc.finalLayout = texture->getImageLayOut();
            attachDesc.samples = VK_SAMPLE_COUNT_1_BIT;
            attachmentDescList.emplace_back(attachDesc);
            
        }

        VkAttachmentReference depthAttachmentRef{};
        depthAttachmentRef.attachment = attachmentDescList.size();
        depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        //depth texture
        auto texture = new DeviceTextureVK();
        texture->initEmpty(width, height, ImageFormat::D24_S8, TextureRtFlagVK::DEPTH_ATTACHEMENT);
        m_depthTexture = texture;
        m_textureList.emplace_back(texture);

        VkSubpassDescription subpassDesc = {};
        subpassDesc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpassDesc.colorAttachmentCount = colorAttachmentRefs.size();
        subpassDesc.pColorAttachments = colorAttachmentRefs.data();
		subpassDesc.pDepthStencilAttachment = &depthAttachmentRef;

        VkAttachmentDescription depthAttachmentDesc{};
        depthAttachmentDesc.format = VK_FORMAT_D24_UNORM_S8_UINT;
        depthAttachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
        depthAttachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        depthAttachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachmentDesc.initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        depthAttachmentDesc.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        attachmentDescList.emplace_back(depthAttachmentDesc);
		
        VkRenderPassCreateInfo renderPassCreateInfo = {};
        renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassCreateInfo.attachmentCount = attachmentDescList.size();
        renderPassCreateInfo.pAttachments = attachmentDescList.data();
        renderPassCreateInfo.subpassCount = 1;
        renderPassCreateInfo.pSubpasses = &subpassDesc;
        
        VkResult res = vkCreateRenderPass(VKRenderBackEnd::shared()->getDevice(), &renderPassCreateInfo, NULL, &m_renderPass);
        if(res != VK_SUCCESS)
        {
            printf("vkCreateRenderPass error %d\n", res);
        }

        printf("Created a render pass\n");

		std::vector<VkImageView> attachments;
        attachments.resize(colorAttachNum + 1);
        for(int i = 0; i <colorAttachNum + 1; i++){
            attachments[i] = m_textureList[i]->getImageView();
        }
		VkFramebufferCreateInfo fbufCreateInfo = {};
		fbufCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		fbufCreateInfo.pNext = NULL;
		fbufCreateInfo.renderPass = m_renderPass;
		fbufCreateInfo.pAttachments = attachments.data();
		fbufCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		fbufCreateInfo.width = width;
		fbufCreateInfo.height = height;
		fbufCreateInfo.layers = 1;
		VK_CHECK_RESULT(vkCreateFramebuffer(VKRenderBackEnd::shared()->getDevice(), &fbufCreateInfo, nullptr, &m_frameBuffer));
	}
    VkRenderPass DeviceRenderPassVK::getRenderPass()
    {
        return m_renderPass;
    }
    VkFramebuffer DeviceRenderPassVK::getFrameBuffer()
    {
        return m_frameBuffer;
    }
    std::vector<DeviceTextureVK*>& DeviceRenderPassVK::getTextureList()
    {
        return m_textureList;
    }

    DeviceTextureVK * DeviceRenderPassVK::getDepthMap()
    {
    
        return m_depthTexture;
    
    }
}