#include "DeviceRenderPassVK.h"
#include "../VkRenderBackEnd.h"
#include "DeviceTextureVK.h"
namespace tzw
{
	DeviceRenderPassVK::DeviceRenderPassVK(int colorAttachNum, OpType opType, ImageFormat format)
	{
        m_opType = opType;
        std::vector<VkAttachmentReference> colorAttachmentRefs;
        std::vector<VkAttachmentDescription> attachmentDescList;
        for(int i =0 ; i < colorAttachNum; i++)
        {
            VkAttachmentReference attachRef = {};
            attachRef.attachment = i;
            attachRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            colorAttachmentRefs.emplace_back(attachRef);

            DeviceRenderPassAttachmentInfo attachInfo;
            attachInfo.attachmentIndex = i;
            attachInfo.format = format;
            attachInfo.isDepth = false;
            m_attachmentList.emplace_back(attachInfo);
            
            VkAttachmentDescription attachDesc = {};    
            attachDesc.format = VKRenderBackEnd::shared()->getFormat(format);
            switch(m_opType)
            {
                case OpType::LOAD_AND_STORE:
                {
                    attachDesc.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
                    attachDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
                }
                break;
                case OpType::CLEAR_AND_STORE:
                {
                    attachDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
                    attachDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
                }
                break;
            }
            attachDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            attachDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            attachDesc.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            attachDesc.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            attachDesc.samples = VK_SAMPLE_COUNT_1_BIT;
            attachmentDescList.emplace_back(attachDesc);
            
        }

        VkAttachmentReference depthAttachmentRef{};
        depthAttachmentRef.attachment = attachmentDescList.size();
        depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        DeviceRenderPassAttachmentInfo depthAttachInfo;
        depthAttachInfo.attachmentIndex = depthAttachmentRef.attachment;
        depthAttachInfo.isDepth = true;
        depthAttachInfo.format = ImageFormat::D24_S8;
        m_attachmentList.emplace_back(depthAttachInfo);

        VkSubpassDescription subpassDesc = {};
        subpassDesc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpassDesc.colorAttachmentCount = colorAttachmentRefs.size();
        subpassDesc.pColorAttachments = colorAttachmentRefs.data();
		subpassDesc.pDepthStencilAttachment = &depthAttachmentRef;

        VkAttachmentDescription depthAttachmentDesc{};
        depthAttachmentDesc.format = VK_FORMAT_D24_UNORM_S8_UINT;
        depthAttachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
        switch(m_opType)
        {
            case OpType::LOAD_AND_STORE:
            {
                depthAttachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
                depthAttachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            }
            break;
            case OpType::CLEAR_AND_STORE:
            {
                depthAttachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
                depthAttachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            }
            break;
        }
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
	}
    VkRenderPass DeviceRenderPassVK::getRenderPass()
    {
        return m_renderPass;
    }

    size_t DeviceRenderPassVK::getAttachmentCount()
    {
        return m_attachmentList.size();
    }
    std::vector<DeviceRenderPassAttachmentInfo>& DeviceRenderPassVK::getAttachmentList()
    {
        return m_attachmentList;
    }

    DeviceRenderPassVK::OpType DeviceRenderPassVK::getOpType()
    {
        return m_opType;
    
    }
}