#include "DeviceShaderVK.h"
#include <cassert>
#include "../VkRenderBackEnd.h"
#define NOMINMAX 1
#include "spirv_cross.hpp"
#include "spirv_glsl.hpp"
#include <iostream>
namespace tzw{
void DeviceShaderVK::addShader(const unsigned char* buff, size_t size, DeviceShaderType type, const unsigned char* fileInfoStr)
{
	auto pShaderCode = buff;//ReadBinaryFile(pFileName, codeSize);
    int codeSize = size;
    assert(pShaderCode);
  
    VkShaderModuleCreateInfo shaderCreateInfo = {};
    shaderCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderCreateInfo.codeSize = codeSize;
    shaderCreateInfo.pCode = (const uint32_t*)pShaderCode;
    
    try{
    
    spirv_cross::Compiler  glsl((const uint32_t*)pShaderCode, codeSize / sizeof(uint32_t));
	// The SPIR-V is now parsed, and we can perform reflection on it.
	spirv_cross::ShaderResources resources = glsl.get_shader_resources();

	// Get all sampled images in the shader.
	for (auto &resource : resources.sampled_images)
	{
		unsigned set = glsl.get_decoration(resource.id, spv::DecorationDescriptorSet);
		unsigned binding = glsl.get_decoration(resource.id, spv::DecorationBinding);
		printf("Image %s at set = %u, binding = %u\n", resource.name.c_str(), set, binding);
        DeviceShaderVKLocationInfo info;
        info.set = set;
        info.binding = binding;
        info.type = DeviceShaderVKLocationType::Sampler;
        if(type == DeviceShaderType::VertexShader){
            info.stageFlag |= VK_SHADER_STAGE_VERTEX_BIT;
        }
        else{
        
            info.stageFlag |= VK_SHADER_STAGE_FRAGMENT_BIT;
        }
        m_nameInfoMap[resource.name.c_str()] = info;
	}
	for (auto &resource : resources.uniform_buffers)
	{
		unsigned set = glsl.get_decoration(resource.id, spv::DecorationDescriptorSet);
		unsigned binding = glsl.get_decoration(resource.id, spv::DecorationBinding);
		printf("Uniform %s at set = %u, binding = %u\n", resource.name.c_str(), set, binding);
        DeviceShaderVKLocationInfo info;
        info.set = set;
        info.binding = binding;
        info.type = DeviceShaderVKLocationType::Uniform;
        if(type == DeviceShaderType::VertexShader){
            info.stageFlag |= VK_SHADER_STAGE_VERTEX_BIT;
        }
        else{
        
            info.stageFlag |= VK_SHADER_STAGE_FRAGMENT_BIT;
        }
        m_nameInfoMap[resource.name.c_str()] = info;
	}
    }
    catch(const spirv_cross::CompilerError &e){
    
        std::cout<<(e.what());
        abort();
    }
    VkShaderModule * shaderModule = (VkShaderModule *)malloc(sizeof(shaderModule));
    VkResult res = vkCreateShaderModule(VKRenderBackEnd::shared()->getDevice(), &shaderCreateInfo, NULL, shaderModule);
    printf("vkCreateShaderModule error %d\n", res);
    printf("Created shader %s\n", fileInfoStr);
    switch(type)
    {
    case DeviceShaderType::VertexShader:
        m_vsShader = shaderModule;
    break;
    case DeviceShaderType::FragmentShader:
        m_fsShader = shaderModule;
    break;
    case DeviceShaderType::TessControlShader:
        m_tsShader = shaderModule;
    break;
    case DeviceShaderType::TessEvaulateShader:
        m_teShader = shaderModule;
    break;
    }
}

bool DeviceShaderVK::create()
{
    m_vsShader = nullptr;
    m_fsShader = nullptr;
    m_tsShader = nullptr;
    m_teShader = nullptr;
	return true;
}

bool DeviceShaderVK::finish()
{
    createDescriptorSetLayOut();
	return true;
}

VkShaderModule* DeviceShaderVK::getVsModule()
{
    return m_vsShader;
}

VkShaderModule* DeviceShaderVK::getFsModule()
{
    return m_fsShader;
}

std::unordered_map<std::string, DeviceShaderVKLocationInfo>& DeviceShaderVK::getNameLocationMap()
{
    return m_nameInfoMap;
}

void DeviceShaderVK::createDescriptorSetLayOut()
{
    auto shader = this;
    auto& locationMap = shader->getNameLocationMap();
    std::vector<VkDescriptorSetLayoutBinding> descriptorLayoutList;
    for(auto & locationInfo : locationMap){
        VkDescriptorSetLayoutBinding layOutBinding{};
        layOutBinding.binding = locationInfo.second.binding;
        if(locationInfo.second.type == DeviceShaderVKLocationType::Uniform)
        {
            layOutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        }
        else if(locationInfo.second.type == DeviceShaderVKLocationType::Sampler)
        {
            layOutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        }
        
        layOutBinding.descriptorCount = 1;
        layOutBinding.stageFlags = locationInfo.second.stageFlag;
        descriptorLayoutList.emplace_back(layOutBinding);
    }
    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = descriptorLayoutList.size();
    layoutInfo.pBindings = descriptorLayoutList.data();
    auto res = vkCreateDescriptorSetLayout(VKRenderBackEnd::shared()->getDevice(), &layoutInfo, nullptr, &m_descriptorSetLayout);
    assert(!res);
}

VkDescriptorSetLayout& DeviceShaderVK::getDescriptorSetLayOut()
{
    return m_descriptorSetLayout;
}

}
