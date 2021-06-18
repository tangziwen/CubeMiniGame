#include "DevicePipelineVK.h"
#include "Technique/Material.h"
#include "DeviceShaderVK.h"
#include "Mesh/VertexData.h"
#include "Engine/Engine.h"
#include "../VkRenderBackEnd.h"
#include "DeviceShaderVK.h"
#include "DeviceTextureVK.h"
#include <array>
#include "Scene/SceneMgr.h"
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))
#define CHECK_VULKAN_ERROR(a, b) {if(b){printf(a, b);abort();}}
static const unsigned int DescriptorGuessCount = 512; 
namespace tzw
{

DevicePipelineVK::DevicePipelineVK()
{
	
}


void DevicePipelineVK::init(vec2 viewPortSize, Material* mat, DeviceRenderPass* targetRenderPass, DeviceVertexInput vertexInput, bool isSupportInstancing, DeviceVertexInput instanceVertexInput, int colorAttachmentCount)
{

    m_totalItemWiseDesSet = 0;
    m_vertexInput = vertexInput;
    m_mat = mat;
    DeviceShaderVK * shader = static_cast<DeviceShaderVK *>(mat->getProgram()->getDeviceShader());
    m_shader = shader;
    //create material descriptor pool
    createMaterialDescriptorPool();
    crateMaterialDescriptorSet();
    createMaterialUniformBuffer();
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

    std::vector<VkVertexInputBindingDescription> bindingDescriptionList;
    //create vertex input
    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding = 0;
    bindingDescription.stride = m_vertexInput.stride;
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    bindingDescriptionList.emplace_back(bindingDescription);


    VkVertexInputBindingDescription instancingBindingDescription{};

    
    if(isSupportInstancing)
    {
        instancingBindingDescription.binding = 1;
        instancingBindingDescription.stride = instanceVertexInput.stride;
        instancingBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;
        bindingDescriptionList.emplace_back(instancingBindingDescription);
    }
    
    std::vector<VkVertexInputAttributeDescription> attributeDecsriptionList;
    if(isSupportInstancing)
    {
        attributeDecsriptionList.resize(m_vertexInput.m_attributeList.size() + instanceVertexInput.m_attributeList.size());
    }
    else{
    
        attributeDecsriptionList.resize(m_vertexInput.m_attributeList.size());
    }
    

    for(int i = 0; i < m_vertexInput.m_attributeList.size(); i++)
    {
        attributeDecsriptionList[i].binding = 0;
        attributeDecsriptionList[i].location = i;
        attributeDecsriptionList[i].format = m_vertexInput.m_attributeList[i].format;
        attributeDecsriptionList[i].offset = m_vertexInput.m_attributeList[i].offset;
    }
    if(isSupportInstancing){
        size_t normalVertexSize = m_vertexInput.m_attributeList.size();
        for(int i = normalVertexSize; i < instanceVertexInput.m_attributeList.size() + normalVertexSize; i++)
        {
            attributeDecsriptionList[i].binding = 1;
            attributeDecsriptionList[i].location = i;
            attributeDecsriptionList[i].format = instanceVertexInput.m_attributeList[i - normalVertexSize].format;
            attributeDecsriptionList[i].offset = instanceVertexInput.m_attributeList[i - normalVertexSize].offset;
        }
    }

    vertexInputInfo.vertexBindingDescriptionCount = bindingDescriptionList.size();
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDecsriptionList.size());
    vertexInputInfo.pVertexBindingDescriptions = bindingDescriptionList.data();
    vertexInputInfo.pVertexAttributeDescriptions = attributeDecsriptionList.data();

    VkPipelineInputAssemblyStateCreateInfo pipelineIACreateInfo = {};
    pipelineIACreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    switch(mat->getPrimitiveTopology())
    {
    case PrimitiveTopology::TriangleList:
        pipelineIACreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        break;
    case PrimitiveTopology::LineList:
        pipelineIACreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
        break;
    default:
        break;
    }
    //auto screenSize = Engine::shared()->winSize();
    VkViewport vp = {};
    vp.x = 0.0f;
    vp.y = 0.0f;
    vp.width  = (float)viewPortSize.x;
    vp.height = (float)viewPortSize.y;
    vp.minDepth = 0.0f;
    vp.maxDepth = 1.0f;
        
    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = VkExtent2D{(uint32_t)viewPortSize.x, (uint32_t)viewPortSize.y};

    VkPipelineViewportStateCreateInfo vpCreateInfo = {};
    vpCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    vpCreateInfo.viewportCount = 1;
    vpCreateInfo.pViewports = &vp;
    vpCreateInfo.scissorCount = 1;
    vpCreateInfo.pScissors = &scissor;

	VkPipelineDynamicStateCreateInfo vpDynamicStateInfo = {};
	std::vector<VkDynamicState> dynamicStateArray;
	if(m_dynamicState)
	{
		if(m_dynamicState & PIPELINE_DYNAMIC_STATE_FLAG_SCISSOR)
		{
			 dynamicStateArray.emplace_back(VK_DYNAMIC_STATE_SCISSOR);
		}
		if(m_dynamicState & PIPELINE_DYNAMIC_STATE_FLAG_VIEWPORT)
		{
			 dynamicStateArray.emplace_back(VK_DYNAMIC_STATE_VIEWPORT);
		}
		vpDynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		vpDynamicStateInfo.dynamicStateCount = dynamicStateArray.size();
		vpDynamicStateInfo.pDynamicStates = dynamicStateArray.data();
	}

	
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
        RenderFlag::CullMode cullMode =  mat->getCullMode();
        if(cullMode == RenderFlag::CullMode::Back)
        {
            rastCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
        }
        else if(cullMode == RenderFlag::CullMode::Front)
        {
            rastCreateInfo.cullMode = VK_CULL_MODE_FRONT_BIT;
        }else
        {
            rastCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
        }  
    }
    else{
        rastCreateInfo.cullMode = VK_CULL_MODE_NONE;
    }
    rastCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rastCreateInfo.lineWidth = 1.0f;
    
    VkPipelineMultisampleStateCreateInfo pipelineMSCreateInfo = {};
    pipelineMSCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    pipelineMSCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    pipelineMSCreateInfo.flags = 0;
    
    std::vector<VkPipelineColorBlendAttachmentState> blendStateList;
    for(int i = 0; i < colorAttachmentCount; i++)
    {
        VkPipelineColorBlendAttachmentState blendAttachState = {};
        blendAttachState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        blendAttachState.srcColorBlendFactor= getBlendFactor(m_mat->getFactorSrc());
        blendAttachState.dstColorBlendFactor=getBlendFactor(m_mat->getFactorDst());
        blendAttachState.colorBlendOp=VK_BLEND_OP_ADD;

        blendAttachState.srcAlphaBlendFactor=VK_BLEND_FACTOR_ONE;
        blendAttachState.dstAlphaBlendFactor=VK_BLEND_FACTOR_ZERO;
        blendAttachState.alphaBlendOp=VK_BLEND_OP_ADD;
        blendAttachState.blendEnable = m_mat->isIsEnableBlend();

        blendStateList.emplace_back(blendAttachState);
    
    }

    
    VkPipelineColorBlendStateCreateInfo blendCreateInfo = {};
    blendCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    blendCreateInfo.logicOp = VK_LOGIC_OP_COPY;
    blendCreateInfo.attachmentCount = blendStateList.size();
    blendCreateInfo.pAttachments = blendStateList.data();
 



    //pipeline layout
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    std::vector<VkDescriptorSetLayout> layOutList = {shader->getMaterialDescriptorSetLayOut(), };
    if(!shader->isHavePerObjectDescriptorSetLayOut())
    {
        pipelineLayoutInfo.setLayoutCount = layOutList.size();
        pipelineLayoutInfo.pSetLayouts = layOutList.data();
    }
    else
    {
        layOutList.emplace_back( shader->getDescriptorSetLayOut());
        pipelineLayoutInfo.setLayoutCount = layOutList.size();
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
    pipelineInfo.renderPass = static_cast<DeviceRenderPassVK *>(targetRenderPass)->getRenderPass();
    pipelineInfo.basePipelineIndex = -1;
	if(m_dynamicState)
	{
		pipelineInfo.pDynamicState = &vpDynamicStateInfo;
	}
    
    VkResult res = vkCreateGraphicsPipelines(VKRenderBackEnd::shared()->getDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, NULL, &m_pipeline);
    CHECK_VULKAN_ERROR("vkCreateGraphicsPipelines error %d\n", res);
    
    printf("Graphics pipeline created\n");


    updateMaterialDescriptorSet();//only update once
    updateUniform();//will be update every frame, or every change.
    printf("pipeline create %p\n", m_pipeline);

    m_itemBufferPool = new DeviceItemBufferPoolVK(1024 * 1024 * 5);
}
VkDescriptorSetLayout DevicePipelineVK::getDescriptorSetLayOut()
{
    DeviceShaderVK * shader = static_cast<DeviceShaderVK *>(m_mat->getProgram()->getDeviceShader());
    return shader->getDescriptorSetLayOut();
}

VkDescriptorSetLayout DevicePipelineVK::getMaterialDescriptorSetLayOut()
{
    DeviceShaderVK * shader = static_cast<DeviceShaderVK *>(m_mat->getProgram()->getDeviceShader());
    return shader->getMaterialDescriptorSetLayOut();
}

VkPipelineLayout DevicePipelineVK::getPipelineLayOut()
{
    return m_pipelineLayout;
}

VkPipeline DevicePipelineVK::getPipeline()
{
    return m_pipeline;
}

DeviceDescriptor* DevicePipelineVK::getMaterialDescriptorSet()
{

    return m_materialDescripotrSet;
}

void DevicePipelineVK::updateMaterialDescriptorSet()
{
    DeviceShaderVK * shader = static_cast<DeviceShaderVK *>(m_mat->getProgram()->getDeviceShader());
    auto setInfo = shader->getSetInfo();
    auto matDescIter = setInfo.find(0);
    if(matDescIter == setInfo.end())
    {
        
        return;
    }
    auto & matDescSet = matDescIter->second;
    auto & varList = m_mat->getVarList();


    for(auto& i :matDescSet)
    {
        switch(i.type)
        {
            //currently the uniform descriptor only can show once, and always be called "t_shaderUnifom"
            case DeviceShaderVKLocationType::Uniform:
            {
                //update materials uniform buffer
                VkDescriptorBufferInfo bufferInfo{};
                bufferInfo.buffer = m_matUniformBuffer;
                bufferInfo.offset = 0;
                bufferInfo.range = i.size;

                VkWriteDescriptorSet writeSet{};
                writeSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                writeSet.dstSet = m_materialDescripotrSet->getDescSet();
                writeSet.dstBinding = i.binding;
                writeSet.dstArrayElement = 0;
                writeSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                writeSet.descriptorCount = 1;
                writeSet.pBufferInfo = &bufferInfo;
                vkUpdateDescriptorSets(VKRenderBackEnd::shared()->getDevice(), 1, &writeSet, 0, nullptr);
            }
            break;
            case DeviceShaderVKLocationType::Sampler:
            {
                auto iter = varList.find(i.name);
                if(iter != varList.end())
                {

                    TechniqueVar* var = &(iter->second);
                    assert(var->type == TechniqueVar::Type::Texture);
                    auto tex = var->data.rawData.texInfo.tex;
                    VkDescriptorImageInfo imageInfo{};
                    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                    imageInfo.imageView = static_cast<DeviceTextureVK *>(tex->getTextureId())->getImageView();
                    imageInfo.sampler = static_cast<DeviceTextureVK *>(tex->getTextureId())->getSampler();


                    VkWriteDescriptorSet texWriteSet{};
                    texWriteSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                    texWriteSet.dstSet = m_materialDescripotrSet->getDescSet();
                    texWriteSet.dstBinding = i.binding;
                    texWriteSet.dstArrayElement = 0;
                    texWriteSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                    texWriteSet.descriptorCount = 1;
                    texWriteSet.pImageInfo = &imageInfo;
                    vkUpdateDescriptorSets(VKRenderBackEnd::shared()->getDevice(), 1, &texWriteSet, 0, nullptr);
                }
            }
            break;
        
        }
    
    
    }
    return;
}

void DevicePipelineVK::updateUniform()
{
    DeviceShaderVK * shader = m_shader;
    if(!shader->findLocationInfo("t_shaderUnifom")) return;
    auto materialUniformBufferInfo = shader->getLocationInfo("t_shaderUnifom");
    //update material parameter
    auto & varList = m_mat->getVarList();
    std::vector<VkWriteDescriptorSet> descriptorWrites{};
    void* data;
    //copy new data
    vkMapMemory(VKRenderBackEnd::shared()->getDevice(), m_matUniformBufferMemory, 0, sizeof(materialUniformBufferInfo.size), 0, &data);

    for(int idx = 0; idx < materialUniformBufferInfo.m_member.size(); idx++)
    {
        auto & blockMember = materialUniformBufferInfo.m_member[idx];
        auto iter = varList.find(blockMember.name);
        if(iter !=varList.end())
        {
            void * offsetDst = (char *)data + blockMember.offset;
            TechniqueVar* var = &iter->second;
            switch(var->type)
            {
                case TechniqueVar::Type::Vec4:
                {
                    memcpy(offsetDst, &(var->data.rawData.v4), blockMember.size);
                    break;
                }
                case TechniqueVar::Type::Float:
                {

                    memcpy(offsetDst, &(var->data.rawData.f), blockMember.size);

                    break;
                }
                case TechniqueVar::Type::Integer:
                {
                    memcpy(offsetDst, &(var->data.rawData.i), blockMember.size);
                    break;
                }
                case TechniqueVar::Type::Vec2:
                {
                    memcpy(offsetDst, &(var->data.rawData.v2), blockMember.size);
                    break;
                }
                case TechniqueVar::Type::Vec3:
                {
                    memcpy(offsetDst, &(var->data.rawData.v3), blockMember.size);
                    break;
                }
                case TechniqueVar::Type::Semantic:
                {
                    switch(var->semantic)
                    {
		                case TechniqueVar::SemanticType::NO_SEMANTIC: break;
                        case TechniqueVar::SemanticType::WIN_SIZE:
		                {
                            vec2 winSize = Engine::shared()->winSize();
                            memcpy(offsetDst, &winSize, blockMember.size);
                        }
		                break;
		                case TechniqueVar::SemanticType::ModelViewProj: break;
		                case TechniqueVar::SemanticType::Model: break;
		                case TechniqueVar::SemanticType::View: break;
		                case TechniqueVar::SemanticType::Project: break;
		                case TechniqueVar::SemanticType::InvertedProj: break;
		                case TechniqueVar::SemanticType::CamPos:
		                {
                            if(g_GetCurrScene() && g_GetCurrScene()->defaultCamera())
                            {
                                vec3 worldPos = g_GetCurrScene()->defaultCamera()->getWorldPos();
                                memcpy(offsetDst, &worldPos, blockMember.size);
                            }

		                }
		                break;
                        case TechniqueVar::SemanticType::InvertedViewProj:
		                {
                            if(g_GetCurrScene() && g_GetCurrScene()->defaultCamera())
                            {
			                    auto currScene = g_GetCurrScene();
			                    auto cam = currScene->defaultCamera();
                                auto invertedMat = cam->getViewProjectionMatrix().inverted();
                                memcpy(offsetDst, invertedMat.data(), blockMember.size);
                            }

                        }
		                break;
		                case TechniqueVar::SemanticType::CamDir:
		                {
                            if(g_GetCurrScene() && g_GetCurrScene()->defaultCamera())
                            {
                                vec3 camDir = g_GetCurrScene()->defaultCamera()->getForward();
                                memcpy(offsetDst, &camDir, blockMember.size);
                            }
		                }
		                break;
		                case TechniqueVar::SemanticType::SunDirection:
		                {
                            if(g_GetCurrScene())
                            {
                                auto dirLight = g_GetCurrScene()->getDirectionLight();
                                vec3 sunDir = dirLight->dir();
                                memcpy(offsetDst, &sunDir, blockMember.size);
                            }
		                }
		                break;
		                case TechniqueVar::SemanticType::SunColor:
		                {
                            if(g_GetCurrScene())
                            {
                                auto dirLight = g_GetCurrScene()->getDirectionLight();
                                vec3 sunColor = dirLight->getLightColor() * dirLight->intensity();
                                memcpy(offsetDst, &sunColor, blockMember.size);
                            }
		                }
		                break;
		                default: ;
                    }
                }
                break;
            }
        }
    }

     vkUnmapMemory(VKRenderBackEnd::shared()->getDevice(), m_matUniformBufferMemory);
}

void DevicePipelineVK::updateUniformSingle(std::string name, void* buff, size_t size)
{
    DeviceShaderVK * shader = m_shader;
    if(!shader->findLocationInfo("t_shaderUnifom")) return;
    auto materialUniformBufferInfo = shader->getLocationInfo("t_shaderUnifom");
    int idx = materialUniformBufferInfo.getBlockMemberIndex(name);
    void * data;
    vkMapMemory(VKRenderBackEnd::shared()->getDevice(), m_matUniformBufferMemory, 0, sizeof(materialUniformBufferInfo.size), 0, &data);
    auto & blockMember = materialUniformBufferInfo.m_member[idx];
    void * offsetDst = (char *)data + blockMember.offset;
    if(size > blockMember.size){
        printf("excepted size is %d, current is %d!!!", size, blockMember.size);
        abort();
    }
    memcpy(offsetDst, buff, size);
    vkUnmapMemory(VKRenderBackEnd::shared()->getDevice(), m_matUniformBufferMemory);
}

void DevicePipelineVK::resetItemWiseDescritporSet()
{
    m_currItemWiseDescriptorSetIdx = 0;
}

DeviceDescriptor * DevicePipelineVK::giveItemWiseDescriptorSet()
{
    DeviceDescriptorVK * target;
    if(m_currItemWiseDescriptorSetIdx< m_itemDescritptorSetList.size()){
    
        target = m_itemDescritptorSetList[m_currItemWiseDescriptorSetIdx];
    }
    else
    {
    	if(m_currItemWiseDescriptorSetIdx >= m_totalItemWiseDesSet)
    	{
    		//create another descriptor pool
    		createDescriptorPool();
    	}
        auto layOut = m_shader->getDescriptorSetLayOut();
        //create DescriptorSet
        VkDescriptorSet descriptorSet;
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = m_descriptorPoolList.back();
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &layOut;

        auto res = vkAllocateDescriptorSets(VKRenderBackEnd::shared()->getDevice(), &allocInfo, &descriptorSet);
        if ( res!= VK_SUCCESS) {
            printf("bad  descriptor!!!! %d",res);
            abort();
        }
        target = new DeviceDescriptorVK(descriptorSet, m_shader->getLayOutBySet(1));
        m_itemDescritptorSetList.emplace_back(target);
    }
    m_currItemWiseDescriptorSetIdx ++;
    return target;
}

Material* DevicePipelineVK::getMat()
{
    return m_mat;
}

DeviceRenderItem* DevicePipelineVK::getRenderItem(void* obj)
{
    auto result = m_renderItemMap.find(obj);
    if(result != m_renderItemMap.end())
    {
        return result->second;
    }
    else
    {
        DeviceDescriptor * itemDescriptorSet = giveItemWiseDescriptorSet();
        DeviceItemBuffer itemBuf = m_itemBufferPool->giveMeItemBuffer(sizeof(ItemUniform));
        DeviceRenderItem * item = new DeviceRenderItem();
        item->ptr = obj;
        item->m_buff = itemBuf;
        item->m_desc = itemDescriptorSet;
        m_renderItemMap[obj]= item;
        itemDescriptorSet->updateDescriptorByBinding(0, &itemBuf);
        return item;
    }
    //return nullptr;
}

void DevicePipelineVK::createDescriptorPool()
{
    //每个shader必有两个descriptorSet存在的情况必然是1比1，所以加起来来预估一个池比例
    unsigned uniformBuffCount = 0;
    unsigned textuerCount = 0;
    auto& setInfo = m_shader->getSetInfo();
    for(const auto& iter : setInfo)
    {
        for(auto & locationInfo : iter.second){
        	if(locationInfo.set != OBJECT_DESCRIPTOR_SET_ID) continue;
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

	std::array<VkDescriptorPoolSize, 2> poolSizes{};
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = std::max(DescriptorGuessCount * uniformBuffCount, (unsigned)1);

	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = std::max(DescriptorGuessCount * textuerCount, (unsigned)1);

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = DescriptorGuessCount;
	VkDescriptorPool descriptorPool;
    m_totalItemWiseDesSet += DescriptorGuessCount;
    if (vkCreateDescriptorPool(VKRenderBackEnd::shared()->getDevice(), &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
        abort();
    }
	m_descriptorPoolList.emplace_back(descriptorPool);
}

void DevicePipelineVK::createMaterialDescriptorPool()
{
    unsigned uniformBuffCount = 0;
    unsigned textuerCount = 0;
    auto& setInfo = m_shader->getSetInfo();
    for(const auto& iter : setInfo)
    {
        for(auto & locationInfo : iter.second){
        	if(locationInfo.set != MATERIAL_DESCRIPTOR_SET_ID) continue;
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

	std::array<VkDescriptorPoolSize, 2> poolSizes{};
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = std::max(1 * uniformBuffCount, (unsigned)1);

	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = std::max(1 * textuerCount, (unsigned)1);

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = 1;
    if (vkCreateDescriptorPool(VKRenderBackEnd::shared()->getDevice(), &poolInfo, nullptr, &m_materialDescriptorPool) != VK_SUCCESS) {
        abort();
    }
    
}

void DevicePipelineVK::createMaterialUniformBuffer()
{
    DeviceShaderVK * shader = static_cast<DeviceShaderVK *>(m_mat->getProgram()->getDeviceShader());
    //create material-wise uniform buffer
    if(shader->hasLocationInfo("t_shaderUnifom"))
    {
        auto uniformInfo = shader->getLocationInfo("t_shaderUnifom");
        VKRenderBackEnd::shared()->createVKBuffer(uniformInfo.size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_matUniformBuffer, m_matUniformBufferMemory);
    }
}

void DevicePipelineVK::crateMaterialDescriptorSet()
{
    //CreateDescriptor
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = m_materialDescriptorPool;
    auto layout = m_shader->getMaterialDescriptorSetLayOut();
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &layout;
    VkDescriptorSet descriptorSet;
    auto res = vkAllocateDescriptorSets(VKRenderBackEnd::shared()->getDevice(), &allocInfo, &descriptorSet);
    m_materialDescripotrSet = new DeviceDescriptorVK(descriptorSet, m_shader->getLayOutBySet(0));
    if(res!= VK_SUCCESS)
    {
        abort();
    }
    printf("create descriptor sets 33333333%p\n", m_materialDescripotrSet);
}

VkBlendFactor DevicePipelineVK::getBlendFactor(RenderFlag::BlendingFactor factor)
{
    switch (factor)
    {
    case RenderFlag::BlendingFactor::One:
        return VK_BLEND_FACTOR_ONE;
    case RenderFlag::BlendingFactor::Zero:
        return VK_BLEND_FACTOR_ZERO;
    case RenderFlag::BlendingFactor::SrcAlpha:
        return VK_BLEND_FACTOR_SRC_ALPHA;
    case RenderFlag::BlendingFactor::OneMinusSrcAlpha:
        return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    case RenderFlag::BlendingFactor::ConstantAlpha:
        return VK_BLEND_FACTOR_CONSTANT_ALPHA;
    default:
        return VK_BLEND_FACTOR_ONE;
    }
}

}
