#include "DevicePipelineVK.h"
#include "Technique/Material.h"
#include "DeviceShaderVK.h"
#include "Mesh/VertexData.h"
#include "Engine/Engine.h"
#include "../VkRenderBackEnd.h"
#include "DeviceShaderVK.h"
#include "DeviceTextureVK.h"
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

DevicePipelineVK::DevicePipelineVK(Material* mat, VkRenderPass targetRenderPass ,DeviceVertexInput vertexInput)
{
    m_vertexInput = vertexInput;
    m_mat = mat;
    DeviceShaderVK * shader = static_cast<DeviceShaderVK *>(mat->getProgram()->getDeviceShader());
    m_shader = shader;
    //create descriptor pool
    createDescriptorPool();
    bool isHaveMaterialDescripotrSet = false;
    if(shader->isHaveMaterialDescriptorSetLayOut())
    {
        isHaveMaterialDescripotrSet = true;
        crateMaterialDescriptorSet();
        createMaterialUniformBuffer();
    }
    VkPipelineShaderStageCreateInfo shaderStageCreateInfo[2] = {};
    
    shaderStageCreateInfo[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageCreateInfo[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    shaderStageCreateInfo[0].module = shader->getVsModule();
    shaderStageCreateInfo[0].pName = "main";
    shaderStageCreateInfo[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageCreateInfo[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    shaderStageCreateInfo[1].module = shader->getFsModule();
    shaderStageCreateInfo[1].pName = "main";   
	    
    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    //create vertex input
    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding = 0;
    bindingDescription.stride = m_vertexInput.stride;
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    std::vector<VkVertexInputAttributeDescription> attributeDecsriptionList;
    attributeDecsriptionList.resize(m_vertexInput.m_attributeList.size());

    for(int i = 0; i < m_vertexInput.m_attributeList.size(); i++)
    {
        attributeDecsriptionList[i].binding = 0;
        attributeDecsriptionList[i].location = i;
        attributeDecsriptionList[i].format = m_vertexInput.m_attributeList[i].format;
        attributeDecsriptionList[i].offset = m_vertexInput.m_attributeList[i].offset;
    
    }	


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
    depthStencil.depthTestEnable = mat->isIsDepthTestEnable();
    depthStencil.depthWriteEnable = mat->isIsDepthWriteEnable();
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.stencilTestEnable = VK_FALSE;
	
    VkPipelineRasterizationStateCreateInfo rastCreateInfo = {};
    rastCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rastCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
    if(mat->getIsCullFace()){
    
        rastCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
    }
    else{
        rastCreateInfo.cullMode = VK_CULL_MODE_NONE;
    }
    
    rastCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rastCreateInfo.lineWidth = 1.0f;
    
    VkPipelineMultisampleStateCreateInfo pipelineMSCreateInfo = {};
    pipelineMSCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    
    VkPipelineColorBlendAttachmentState blendAttachState = {};
    blendAttachState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    blendAttachState.srcColorBlendFactor=VK_BLEND_FACTOR_SRC_ALPHA;
    blendAttachState.dstColorBlendFactor=VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    blendAttachState.colorBlendOp=VK_BLEND_OP_ADD;
    blendAttachState.srcAlphaBlendFactor=VK_BLEND_FACTOR_ONE;
    blendAttachState.dstAlphaBlendFactor=VK_BLEND_FACTOR_ZERO;
    blendAttachState.alphaBlendOp=VK_BLEND_OP_ADD;
    blendAttachState.blendEnable = m_mat->isIsEnableBlend();
    
    VkPipelineColorBlendStateCreateInfo blendCreateInfo = {};
    blendCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    blendCreateInfo.logicOp = VK_LOGIC_OP_COPY;
    blendCreateInfo.attachmentCount = 1;
    blendCreateInfo.pAttachments = &blendAttachState;
 



    //pipeline layout
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    if(!isHaveMaterialDescripotrSet)
    {
        pipelineLayoutInfo.setLayoutCount = 1;
        auto descSetlayOut = shader->getDescriptorSetLayOut();
        pipelineLayoutInfo.pSetLayouts = &descSetlayOut;
    }
    else
    {
        std::vector<VkDescriptorSetLayout> layOutList = {shader->getDescriptorSetLayOut(), shader->getMaterialDescriptorSetLayOut()};
        pipelineLayoutInfo.setLayoutCount = layOutList.size();
        auto descSetlayOut = shader->getDescriptorSetLayOut();
        pipelineLayoutInfo.pSetLayouts = layOutList.data();
    }
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


    updateMaterialDescriptorSet();//only update once
    updateUniform();//will be update every frame, or every change.
}

VkDescriptorSetLayout DevicePipelineVK::getDescriptorSetLayOut()
{
    DeviceShaderVK * shader = static_cast<DeviceShaderVK *>(m_mat->getProgram()->getDeviceShader());
    return shader->getDescriptorSetLayOut();
}

VkPipelineLayout DevicePipelineVK::getPipelineLayOut()
{
    return m_pipelineLayout;
}

VkPipeline DevicePipelineVK::getPipeline()
{
    return m_pipeline;
}

VkDescriptorSet DevicePipelineVK::getMaterialDescriptorSet()
{
    // TODO: 在此处插入 return 语句
    return m_materialDescripotrSet;
}

void DevicePipelineVK::updateMaterialDescriptorSet()
{
    DeviceShaderVK * shader = static_cast<DeviceShaderVK *>(m_mat->getProgram()->getDeviceShader());
    if(!shader->findLocationInfo("t_shaderUnifom")) return;

    auto materialUniformBufferInfo = shader->getLocationInfo("t_shaderUnifom");

    //update material parameter
    auto & varList = m_mat->getVarList();
    std::vector<VkWriteDescriptorSet> descriptorWrites{};
    for(auto &i : varList)
    {
        TechniqueVar* var = &i.second;
        switch(var->type)
        {
            case TechniqueVar::Type::Texture:
            {
                auto locationInfo = shader->getLocationInfo(i.first);
                if(locationInfo.set != 1) continue;
                auto tex = var->data.rawData.texInfo.tex;

                VkDescriptorImageInfo imageInfo{};
                imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                imageInfo.imageView = static_cast<DeviceTextureVK *>(tex->getTextureId())->getImageView();
                imageInfo.sampler = static_cast<DeviceTextureVK *>(tex->getTextureId())->getSampler();


                VkWriteDescriptorSet texWriteSet;
                texWriteSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                texWriteSet.dstSet = m_materialDescripotrSet;
                texWriteSet.dstBinding = locationInfo.binding;
                texWriteSet.dstArrayElement = 0;
                texWriteSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                texWriteSet.descriptorCount = 1;
                texWriteSet.pImageInfo = &imageInfo;

                descriptorWrites.emplace_back(texWriteSet);
                break;
            }
        }
    }
    //update materials uniform buffer
    VkDescriptorBufferInfo bufferInfo{};
    bufferInfo.buffer = m_matUniformBuffer;
    bufferInfo.offset = 0;
    bufferInfo.range = materialUniformBufferInfo.size;

    VkWriteDescriptorSet writeSet;
    writeSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeSet.dstSet = m_materialDescripotrSet;
    writeSet.dstBinding = materialUniformBufferInfo.binding;
    writeSet.dstArrayElement = 0;
    writeSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writeSet.descriptorCount = 1;
    writeSet.pBufferInfo = &bufferInfo;
    descriptorWrites.emplace_back(writeSet);


    vkUpdateDescriptorSets(VKRenderBackEnd::shared()->getDevice(), descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
}

void DevicePipelineVK::updateUniform()
{
    DeviceShaderVK * shader = m_shader;
    if(!shader->findLocationInfo("t_shaderUnifom")) return;
    auto materialUniformBufferInfo = shader->getLocationInfo("t_shaderUnifom");
    //update material parameter
    auto & varList = m_mat->getVarList();
    std::vector<VkWriteDescriptorSet> descriptorWrites{};
    for(auto &i : varList)
    {
        TechniqueVar* var = &i.second;
        switch(var->type)
        {
            case TechniqueVar::Type::Vec4:
            {
                int idx = materialUniformBufferInfo.getBlockMemberIndex(i.first);
                if(idx>= 0)
                {
                    auto blockMember = materialUniformBufferInfo.m_member[idx];
                    void* data;
                    //copy new data
                    vkMapMemory(VKRenderBackEnd::shared()->getDevice(), m_matUniformBufferMemory, 0, sizeof(Matrix44), 0, &data);
                        void * offsetDst = (char *)data + blockMember.offset;
                        memcpy(offsetDst, &(var->data.rawData.v4), blockMember.size);
                    vkUnmapMemory(VKRenderBackEnd::shared()->getDevice(), m_matUniformBufferMemory);
                }
                break;
            }
        }
    }
}

void DevicePipelineVK::collcetItemWiseDescritporSet()
{
    m_currItemWiseDescriptorSetIdx = 0;
}

VkDescriptorSet DevicePipelineVK::giveItemWiseDescriptorSet()
{
    VkDescriptorSet target;
    if(m_currItemWiseDescriptorSetIdx< m_itemDescritptorSetList.size()){
    
        target = m_itemDescritptorSetList[m_currItemWiseDescriptorSetIdx];
    }
    else
    {
        auto layOut = m_shader->getDescriptorSetLayOut();
        //create DescriptorSet
        VkDescriptorSet descriptorSet;
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = m_descriptorPool;
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &layOut;

        auto res = vkAllocateDescriptorSets(VKRenderBackEnd::shared()->getDevice(), &allocInfo, &descriptorSet);
        printf("create descriptor sets\n");
        if ( res!= VK_SUCCESS) {
            printf("bad  descriptor!!!! %d",res);
            abort();
        }
        target = descriptorSet;
        m_itemDescritptorSetList.emplace_back(descriptorSet);
    }
    m_currItemWiseDescriptorSetIdx ++;
    return target;
}

Material* DevicePipelineVK::getMat()
{
    return m_mat;
}

void DevicePipelineVK::createDescriptorPool()
{
    //每个shader必有两个descriptorSet存在的情况必然是1比1，所以加起来来预估一个池比例
    unsigned uniformBuffCount = 0;
    unsigned textuerCount = 0;
    auto& setInfo = m_shader->getSetInfo();
    for(auto iter : setInfo)
    {
        for(auto & locationInfo : iter.second){
            if(locationInfo.type == DeviceShaderVKLocationType::Uniform)
            {
                uniformBuffCount ++;
            }
            else if(locationInfo.type == DeviceShaderVKLocationType::Sampler)
            {
                textuerCount++;
            }
        }
    
    }
    unsigned guessCount = 512;
	std::array<VkDescriptorPoolSize, 2> poolSizes{};
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = guessCount * uniformBuffCount;

	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = guessCount * textuerCount;

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = guessCount;

    m_totalItemWiseDesSet = guessCount;
    if (vkCreateDescriptorPool(VKRenderBackEnd::shared()->getDevice(), &poolInfo, nullptr, &m_descriptorPool) != VK_SUCCESS) {
        abort();
    }
}

void DevicePipelineVK::createMaterialUniformBuffer()
{
    DeviceShaderVK * shader = static_cast<DeviceShaderVK *>(m_mat->getProgram()->getDeviceShader());
    //create material-wise uniform buffer
    auto uniformInfo = shader->getLocationInfo("t_shaderUnifom");
    VKRenderBackEnd::shared()->createVKBuffer(uniformInfo.size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_matUniformBuffer, m_matUniformBufferMemory);
}

void DevicePipelineVK::crateMaterialDescriptorSet()
{
    //CreateDescriptor
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = m_descriptorPool;
    auto layout = m_shader->getMaterialDescriptorSetLayOut();
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &layout;

    auto res = vkAllocateDescriptorSets(VKRenderBackEnd::shared()->getDevice(), &allocInfo, &m_materialDescripotrSet);
    if(res!= VK_SUCCESS)
    {
        abort();
    }
}

void DevicePipelineVK::defaultCreateVertexBufferDescription(std::vector<VkVertexInputAttributeDescription>& attributeDescriptions)
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

void DeviceVertexInput::addVertexAttributeDesc(DeviceVertexAttributeDescVK vertexAttributeDesc)
{
    m_attributeList.emplace_back(vertexAttributeDesc);
}

}
