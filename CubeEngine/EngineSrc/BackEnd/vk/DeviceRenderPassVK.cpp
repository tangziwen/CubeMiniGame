#include "DeviceRenderPassVK.h"
#include "../VkRenderBackEnd.h"
#include "DeviceTextureVK.h"
#include <array>
namespace tzw
{
	DeviceRenderPassVK::DeviceRenderPassVK(int colorAttachNum, OpType opType, ImageFormat format,bool isNeedTransitionToRread, bool isOutputToScreen)
        :m_isNeedTransitionToRead(isNeedTransitionToRread),m_isOutPutToScreen(isOutputToScreen)
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
                    if(isOutputToScreen)
                    {
                        attachDesc.initialLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
                    }
                    else
                    {
                        attachDesc.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                    }
                    
                }
                break;
                case OpType::LOADCLEAR_AND_STORE:
                {
                    attachDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
                    attachDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
                    attachDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                }
                break;
            }

            if(isNeedTransitionToRread)
            {
                attachDesc.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            }else
            {
                if(isOutputToScreen){
                    attachDesc.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
                
                }else
                {
                
                    attachDesc.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                }
            }
            attachDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            attachDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

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
                depthAttachmentDesc.initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            }
            break;
            case OpType::LOADCLEAR_AND_STORE:
            {
                depthAttachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
                depthAttachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
                depthAttachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            }
            break;
        }
        depthAttachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        
        if(isNeedTransitionToRread)
        {
            depthAttachmentDesc.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
        }else
        {
            depthAttachmentDesc.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            
        }
        
        attachmentDescList.emplace_back(depthAttachmentDesc);
		


		// Use subpass dependencies for attachment layout transitions
		std::array<VkSubpassDependency, 2> dependencies;

		dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
		dependencies[0].dstSubpass = 0;
		dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

		dependencies[1].srcSubpass = 0;
		dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
		dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

        VkRenderPassCreateInfo renderPassCreateInfo = {};
        renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassCreateInfo.attachmentCount = attachmentDescList.size();
        renderPassCreateInfo.pAttachments = attachmentDescList.data();
        renderPassCreateInfo.subpassCount = 1;
        renderPassCreateInfo.pSubpasses = &subpassDesc;
		//renderPassCreateInfo.dependencyCount = 2;
		//renderPassCreateInfo.pDependencies = dependencies.data();

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