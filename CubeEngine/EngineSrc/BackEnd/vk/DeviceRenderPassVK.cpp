#include "DeviceRenderPassVK.h"
#include "../VkRenderBackEnd.h"
#include "DeviceTextureVK.h"
#include <array>
namespace tzw
{
	DeviceRenderPassVK::DeviceRenderPassVK()
	{
        
	}

    void DeviceRenderPassVK::init(const DeviceAttachmentInfoList & attachList, OpType opType, bool isNeedTransitionToRread, bool isOutputToScreen)
    {
        m_isNeedTransitionToRead = isNeedTransitionToRread;
        m_isOutPutToScreen = isOutputToScreen;
        m_opType = opType;
        std::vector<VkAttachmentReference> colorAttachmentRefs;
        VkAttachmentReference depthAttachmentRef{};
        depthAttachmentRef.attachment = VK_ATTACHMENT_UNUSED;//maybe no used
        std::vector<VkAttachmentDescription> attachmentDescList;
        size_t colorAttachNum = attachList.size();
        for(int i =0 ; i < attachList.size(); i++)
        {


            DeviceRenderPassAttachmentInfo attachInfo = {};
            //attachInfo.attachmentIndex = i;
            ImageFormat format = attachList[i].format;
            attachInfo.format = format;
            attachInfo.isDepthStencilAttachment = attachList[i].isDepthStencilAttachment;
            m_attachmentList.emplace_back(attachInfo);
            if(!attachInfo.isDepthStencilAttachment) // color attach
            {
                VkAttachmentReference colorAttachRef = {};
                colorAttachRef.attachment = i;
                colorAttachRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                colorAttachmentRefs.emplace_back(colorAttachRef);
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
            else// depth attach
            {
                depthAttachmentRef.attachment = i;
                depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
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
            }
        }
        VkSubpassDescription subpassDesc = {};
        subpassDesc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpassDesc.colorAttachmentCount = colorAttachmentRefs.size();
        subpassDesc.pColorAttachments = colorAttachmentRefs.data();
		subpassDesc.pDepthStencilAttachment = &depthAttachmentRef;

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
}