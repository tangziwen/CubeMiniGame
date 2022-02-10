#include <cstdint>
#include "gli/gli.hpp"
#include "DeviceMaterialVK.h"
#include "Technique/Material.h"
#include "DeviceShaderCollectionVK.h"
#include "../VkRenderBackEnd.h"
#include "Engine/Engine.h"
#include "Scene/SceneMgr.h"
#include "DeviceTextureVK.h"
namespace tzw
{

	DeviceMaterialVK::DeviceMaterialVK()
	{

	}

    void DeviceMaterialVK::init(Material* material)
    {
		m_mat = material;
        m_shadingParams = material->getShadingParams();
        DeviceShaderCollectionVK * shader = static_cast<DeviceShaderCollectionVK *>(material->getProgram()->getDeviceShader());
        m_shader = shader;

        //create material descriptor pool
        createMaterialDescriptorPool();
        createMaterialDescriptorSet();
        createMaterialUniformBuffer();

        updateMaterialDescriptorSet();//only update once
        updateUniform();

    }

    void DeviceMaterialVK::initCompute(DeviceShaderCollection* shader)
    {
        DeviceShaderCollectionVK * computeShaderCollection = static_cast<DeviceShaderCollectionVK *>(shader);
        m_shader = computeShaderCollection;
        //create material descriptor pool
        createMaterialDescriptorPool();
        createMaterialDescriptorSet();
        createMaterialUniformBuffer();

        updateMaterialDescriptorSet();//only update once
    }

	DeviceDescriptor* DeviceMaterialVK::getMaterialDescriptorSet()
	{
		return m_materialDescripotrSet;
	}

	void DeviceMaterialVK::updateUniform()
	{
        DeviceShaderCollectionVK * shader = m_shader;
        if(!shader->findLocationInfo("t_shaderUnifom")) return;
        auto materialUniformBufferInfo = shader->getLocationInfo("t_shaderUnifom");
        //update material parameter
        auto & varList = m_shadingParams->getVarList();
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
                    case TechniqueVar::Type::Matrix:
                    {
                        memcpy(offsetDst, &(var->data.rawData.m), blockMember.size);
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
						    case TechniqueVar::SemanticType::CamInfo:
						    {
                                if(g_GetCurrScene())
                                {
							    auto currScene = g_GetCurrScene();
							    auto cam = currScene->defaultCamera();
							    vec4 camInfo(cam->getNear(), cam->getFar(), cam->getFov(), cam->getAspect());
        					    memcpy(offsetDst, &camInfo, blockMember.size);
                                }
				            }
						    break;
		                    default:
                                break;
                        }
                    }
                    break;
                }
            }
        }

         vkUnmapMemory(VKRenderBackEnd::shared()->getDevice(), m_matUniformBufferMemory);
	}

    void DeviceMaterialVK::updateMaterialDescriptorSet()
    {
        DeviceShaderCollectionVK * shader = static_cast<DeviceShaderCollectionVK *>(m_shader);
        auto setInfo = shader->getSetInfo();
        auto matDescIter = setInfo.find(0);
        if(matDescIter == setInfo.end())
        {
        
            return;
        }
        auto & matDescSet = matDescIter->second;
        auto & varList = m_shadingParams->getVarList();


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
                        /*
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
                        */
                        m_materialDescripotrSet->updateDescriptorByBinding(i.binding, static_cast<DeviceTextureVK *>(tex->getTextureId()));
                    }
                }
                break;
        
            }
        }
        return;
    }

    void DeviceMaterialVK::updateUniformSingle(std::string name, void* buff, size_t size)
    {
        DeviceShaderCollectionVK * shader = m_shader;
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

    void DeviceMaterialVK::createMaterialDescriptorPool()
    {
        unsigned uniformBuffCount = 0;
        unsigned textuerCount = 0;
        unsigned storageBufferCount = 0;
        unsigned storageImageCount = 0;
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
                    if(locationInfo.arraySize > 0)
                    {
                        textuerCount += locationInfo.arraySize;
                    }
                    else
                    {
                        textuerCount++;
                    }
                }
                if(locationInfo.type == DeviceShaderVKLocationType::StorageBuffer)
                {
                    storageBufferCount ++;
                }
                if(locationInfo.type == DeviceShaderVKLocationType::StorageImage)
                {
                    storageImageCount ++;
                }
            }
    
        }

	    std::array<VkDescriptorPoolSize, 4> poolSizes{};
	    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	    poolSizes[0].descriptorCount = std::max(1 * uniformBuffCount, (unsigned)1);

	    poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	    poolSizes[1].descriptorCount = std::max(1 * textuerCount, (unsigned)1);

	    poolSizes[2].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	    poolSizes[2].descriptorCount = std::max(1 * storageBufferCount, (unsigned)1);

	    poolSizes[3].type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	    poolSizes[3].descriptorCount = std::max(1 * storageImageCount, (unsigned)1);

	    VkDescriptorPoolCreateInfo poolInfo{};
	    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	    poolInfo.pPoolSizes = poolSizes.data();
	    poolInfo.maxSets = 1;
        if (vkCreateDescriptorPool(VKRenderBackEnd::shared()->getDevice(), &poolInfo, nullptr, &m_materialDescriptorPool) != VK_SUCCESS) {
            abort();
        }
    }

    void DeviceMaterialVK::createMaterialUniformBuffer()
    {
        DeviceShaderCollectionVK * shader = m_shader;//static_cast<DeviceShaderCollectionVK *>(m_mat->getProgram()->getDeviceShader());
        //create material-wise uniform buffer
        if(shader->hasLocationInfo("t_shaderUnifom"))
        {
            auto uniformInfo = shader->getLocationInfo("t_shaderUnifom");
            VKRenderBackEnd::shared()->createVKBuffer(uniformInfo.size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_matUniformBuffer, m_matUniformBufferMemory);
        }
    }

	void DeviceMaterialVK::createMaterialDescriptorSet()
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

}
