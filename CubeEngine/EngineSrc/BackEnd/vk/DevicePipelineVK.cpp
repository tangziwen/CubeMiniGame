#include "DevicePipelineVK.h"
#include "Technique/Material.h"
#include "DeviceShaderVK.h"
#include "Mesh/VertexData.h"
#include "Engine/Engine.h"
#include "../VkRenderBackEnd.h"
#include "DeviceShaderVK.h"
#include <array>
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))
#define CHECK_VULKAN_ERROR(a, b) {if(b){printf(a, b);abort();}}
namespace tzw
{

void CreateVertexBufferDescription(std::vector<VkVertexInputAttributeDescription> & attributeDescriptions)
{

    attributeDescriptions.resize(3);

	//local position
    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(VertexData, m_pos);


	//color
    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(VertexData, m_color);
	
	//uv
	attributeDescriptions[2].binding = 0;
    attributeDescriptions[2].location = 2;
    attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[2].offset = offsetof(VertexData, m_texCoord);

}

DevicePipelineVK::DevicePipelineVK(Material* mat, VkRenderPass targetRenderPass)
{
    m_mat = mat;
    DeviceShaderVK * shader = static_cast<DeviceShaderVK *>(mat->getProgram()->getDeviceShader());
    VkPipelineShaderStageCreateInfo shaderStageCreateInfo[2] = {};
    
    shaderStageCreateInfo[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageCreateInfo[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    shaderStageCreateInfo[0].module = *shader->getVsModule();
    shaderStageCreateInfo[0].pName = "main";
    shaderStageCreateInfo[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageCreateInfo[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    shaderStageCreateInfo[1].module = *shader->getFsModule();
    shaderStageCreateInfo[1].pName = "main";   
	    
    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    //create vertex input
    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(VertexData);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    std::vector<VkVertexInputAttributeDescription> attributeDecsriptionList;
    CreateVertexBufferDescription(attributeDecsriptionList);
		


    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDecsriptionList.size());
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.pVertexAttributeDescriptions = attributeDecsriptionList.data();

    VkPipelineInputAssemblyStateCreateInfo pipelineIACreateInfo = {};
    pipelineIACreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    pipelineIACreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    auto screenSize = Engine::shared()->winSize();
    VkViewport vp = {};
    vp.x = 0.0f;
    vp.y = 0.0f;
    vp.width  = (float)screenSize.x;
    vp.height = (float)screenSize.y;
    vp.minDepth = 0.0f;
    vp.maxDepth = 1.0f;
        
    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = VkExtent2D{(uint32_t)screenSize.x, (uint32_t)screenSize.y};

    VkPipelineViewportStateCreateInfo vpCreateInfo = {};
    vpCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    vpCreateInfo.viewportCount = 1;
    vpCreateInfo.pViewports = &vp;
    vpCreateInfo.scissorCount = 1;
    vpCreateInfo.pScissors = &scissor;


    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_TRUE;
    depthStencil.depthWriteEnable = VK_TRUE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.stencilTestEnable = VK_FALSE;
	
    VkPipelineRasterizationStateCreateInfo rastCreateInfo = {};
    rastCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rastCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
    rastCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
    rastCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rastCreateInfo.lineWidth = 1.0f;
    
    VkPipelineMultisampleStateCreateInfo pipelineMSCreateInfo = {};
    pipelineMSCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    
    VkPipelineColorBlendAttachmentState blendAttachState = {};
    blendAttachState.colorWriteMask = 0xf;
    
    VkPipelineColorBlendStateCreateInfo blendCreateInfo = {};
    blendCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    blendCreateInfo.logicOp = VK_LOGIC_OP_COPY;
    blendCreateInfo.attachmentCount = 1;
    blendCreateInfo.pAttachments = &blendAttachState;
 



    //pipeline layout
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;

    auto descSetlayOut = shader->getDescriptorSetLayOut();
    pipelineLayoutInfo.pSetLayouts = &descSetlayOut;

    if (vkCreatePipelineLayout(VKRenderBackEnd::shared()->getDevice(), &pipelineLayoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS) {
        abort();
    }

    VkGraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = ARRAY_SIZE_IN_ELEMENTS(shaderStageCreateInfo);
    pipelineInfo.pStages = &shaderStageCreateInfo[0];
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &pipelineIACreateInfo;
    pipelineInfo.pViewportState = &vpCreateInfo;
    pipelineInfo.pRasterizationState = &rastCreateInfo;
    pipelineInfo.pMultisampleState = &pipelineMSCreateInfo;
    pipelineInfo.pColorBlendState = &blendCreateInfo;
	pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.layout = m_pipelineLayout;
    pipelineInfo.renderPass = targetRenderPass;
    pipelineInfo.basePipelineIndex = -1;
    
    VkResult res = vkCreateGraphicsPipelines(VKRenderBackEnd::shared()->getDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, NULL, &m_pipeline);
    CHECK_VULKAN_ERROR("vkCreateGraphicsPipelines error %d\n", res);
    
    printf("Graphics pipeline created\n");
}

VkDescriptorSetLayout& DevicePipelineVK::getDescriptorSetLayOut()
{
    DeviceShaderVK * shader = static_cast<DeviceShaderVK *>(m_mat->getProgram()->getDeviceShader());
    return shader->getDescriptorSetLayOut();
}

VkPipelineLayout& DevicePipelineVK::getPipelineLayOut()
{
    return m_pipelineLayout;
}

VkPipeline& DevicePipelineVK::getPipeline()
{
    return m_pipeline;
}

}
