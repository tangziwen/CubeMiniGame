#include "DeviceShaderCollectionVK.h"
#include <cassert>
#include "../VkRenderBackEnd.h"
#define NOMINMAX 1
#include "spirv_cross.hpp"
#include "spirv_glsl.hpp"
#include <iostream>
#include "shaderc/shaderc.hpp"
#include "EngineSrc/Utility/log/Log.h"
#include "Base/TAssert.h"
namespace tzw{
void DeviceShaderCollectionVK::addShader(const unsigned char* buff, size_t size, DeviceShaderType type, const unsigned char* fileInfoStr)
{
    assert(buff);
    DeviceShaderVK * newShader = new DeviceShaderVK();
    newShader->compile(buff, size, type, fileInfoStr);
    switch(type)
    {
    case DeviceShaderType::VertexShader:
        m_vsShader = newShader;
    break;
    case DeviceShaderType::FragmentShader:
        m_fsShader = newShader;
    break;
    case DeviceShaderType::TessControlShader:
        m_tsShader = newShader;
    break;
    case DeviceShaderType::TessEvaulateShader:
        m_teShader = newShader;
    break;
    case DeviceShaderType::ComputeShader:
        m_csShader = newShader;
    break;
    }
}

bool DeviceShaderCollectionVK::create()
{
    m_vsShader = nullptr;
    m_fsShader = nullptr;
    m_tsShader = nullptr;
    m_teShader = nullptr;
    m_csShader = nullptr;
	return true;
}

bool DeviceShaderCollectionVK::finish()
{
    createDescriptorSetLayOut();
	return true;
}

DeviceShaderVK * DeviceShaderCollectionVK::getVsModule()
{
    return m_vsShader;
}

DeviceShaderVK * DeviceShaderCollectionVK::getFsModule()
{
    return m_fsShader;
}
DeviceShaderVK * DeviceShaderCollectionVK::getCsModule()
{
    return m_csShader;
}

DeviceShaderVKLocationInfo DeviceShaderCollectionVK::getLocationInfo(std::string name)
{
    auto result = m_nameInfoMap.find(name);
    if(result == m_nameInfoMap.end())
    {
        tlogError("criticalError%s", name);
        abort();
    }
    return result->second;
}

bool DeviceShaderCollectionVK::hasLocationInfo(std::string name)
{
    auto result = m_nameInfoMap.find(name);
    return (result != m_nameInfoMap.end());
}

bool DeviceShaderCollectionVK::findLocationInfo(std::string name)
{
    return m_nameInfoMap.find(name)!= m_nameInfoMap.end();
}

std::unordered_map<std::string, DeviceShaderVKLocationInfo>& DeviceShaderCollectionVK::getNameLocationMap()
{
    return m_nameInfoMap;
}

//我们的shader是单独编译的，我们需要合并所有描述符的反射信息来构造descriptor
void DeviceShaderCollectionVK::mergeReflection(DeviceShader * shader)
{
    if(!shader) return;



    auto & setInfoMap = shader->getSetInfoMap();
    for (auto everySet :setInfoMap)
    {
        auto result = m_setInfoMap.find(everySet.first);
        if(result != m_setInfoMap.end())
        {
            for(auto & locationInfo : everySet.second)//一个个Descriptor合并
            {
                auto locationQuery = m_nameInfoMap.find(locationInfo.name);
                if(locationQuery == m_nameInfoMap.end())
                {
                    result->second.push_back(locationInfo);
                }
                else
                {
                    //diffrence shader has same name but diffrence descriptor
                    if(locationQuery->second.binding != locationInfo.binding 
                        || locationQuery->second.set != locationInfo.set )
                    {
                        abort();
                    }
                }
            }
        }
        else
        {
            m_setInfoMap[everySet.first] = everySet.second;
            //result->second = everySet.second;
        }
        //if(m_setInfoMap.find()
    }
    //m_setInfoMap.insert(setInfoMap.begin(), setInfoMap.end());

    auto & nameInfoMap = shader->getNameInfoMap();
    m_nameInfoMap.insert(nameInfoMap.begin(), nameInfoMap.end());
}
void DeviceShaderCollectionVK::createDescriptorSetLayOut()
{
    //merge the reflection info
    mergeReflection(m_vsShader);
    mergeReflection(m_fsShader);
    mergeReflection(m_csShader);

    m_descriptorSetLayouts.resize(m_setInfoMap.size());

    for(auto&iter :m_setInfoMap){
        int setIndex = iter.first;
        auto shader = this;
        auto& locationMap = iter.second;//shader->getNameLocationMap();
        auto layout = new DeviceDescriptorSetLayoutVK(setIndex);
        std::vector<VkDescriptorSetLayoutBinding> descriptorLayoutList;
        std::unordered_set<int> bindingSet;
        for(auto & locationInfo : locationMap){
            VkDescriptorSetLayoutBinding layOutBinding{};
            layOutBinding.binding = locationInfo.binding;

            //TAssert(bindingSet.find(locationInfo.binding) == bindingSet.end(), "binding is duplicated");
            if(bindingSet.find(locationInfo.binding) == bindingSet.end())
            {
                bindingSet.insert(locationInfo.binding);

                if(locationInfo.type == DeviceShaderVKLocationType::Uniform)
                {
                    layOutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                }
                else if(locationInfo.type == DeviceShaderVKLocationType::Sampler)
                {
                    layOutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                }
                else if(locationInfo.type == DeviceShaderVKLocationType::StorageBuffer)
                {
                    layOutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
                }
                else if(locationInfo.type == DeviceShaderVKLocationType::StorageImage)
                {
                    layOutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
                }
                if(locationInfo.arraySize > 0)
                {
                    layOutBinding.descriptorCount = locationInfo.arraySize;
                }
                else{
                    layOutBinding.descriptorCount = 1;
                }
                
                layOutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT;//locationInfo.stageFlag;
                descriptorLayoutList.emplace_back(layOutBinding);
                layout->addBinding(locationInfo.binding, locationInfo.name);
            }
        }
        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = descriptorLayoutList.size();
        layoutInfo.pBindings = descriptorLayoutList.data();
        
        VkDescriptorSetLayout outLayout;
        auto res = vkCreateDescriptorSetLayout(VKRenderBackEnd::shared()->getDevice(), &layoutInfo, nullptr, &outLayout);
        if(res != VK_SUCCESS){
        
            abort();
        }
        layout->setLayout(outLayout);
        m_descriptorSetLayouts[setIndex] = layout;

    }
}

VkDescriptorSetLayout DeviceShaderCollectionVK::getDescriptorSetLayOut()
{
    return m_descriptorSetLayouts[1]->getLayout();
}

VkDescriptorSetLayout DeviceShaderCollectionVK::getMaterialDescriptorSetLayOut()
{
    return m_descriptorSetLayouts[0]->getLayout();
}

bool DeviceShaderCollectionVK::isHavePerObjectDescriptorSetLayOut()
{
    return m_setInfoMap.find(1) != m_setInfoMap.end();
}

std::unordered_map<int, std::vector<DeviceShaderVKLocationInfo>>& DeviceShaderCollectionVK::getSetInfo()
{
    return m_setInfoMap;
}

DeviceDescriptorSetLayoutVK* DeviceShaderCollectionVK::getLayOutBySet(unsigned setID)
{
    return m_descriptorSetLayouts[setID];
}



int DeviceShaderVKLocationInfo::getBlockMemberIndex(std::string name)
{
    for(int i = 0; i < m_member.size(); i++)
    {
        if(m_member[i].name == name)
        {
            return i;
        }
    
    }

    return -1;
}

}
