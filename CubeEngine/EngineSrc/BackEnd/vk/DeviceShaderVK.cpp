#include "DeviceShaderVK.h"
#include <cassert>
#include "../VkRenderBackEnd.h"
#define NOMINMAX 1
#include "spirv_cross.hpp"
#include "spirv_glsl.hpp"
#include <iostream>
#include "shaderc/shaderc.hpp"
#include "EngineSrc/Utility/log/Log.h"
namespace tzw{
void DeviceShaderVK::addShader(const unsigned char* buff, size_t size, DeviceShaderType type, const unsigned char* fileInfoStr)
{
    assert(buff);

    //compile GLSL to spir-v
    shaderc::Compiler compiler;
    shaderc::CompileOptions options;
    shaderc_shader_kind compileKind = shaderc_shader_kind::shaderc_glsl_vertex_shader;
    switch(type)
    {
    case DeviceShaderType::VertexShader:
        compileKind = shaderc_shader_kind::shaderc_glsl_vertex_shader;
    break;
    case DeviceShaderType::FragmentShader:
        compileKind = shaderc_shader_kind::shaderc_glsl_fragment_shader;
    break;
    case DeviceShaderType::TessControlShader:
        compileKind = shaderc_shader_kind::shaderc_glsl_tess_control_shader;
    break;
    case DeviceShaderType::TessEvaulateShader:
        compileKind = shaderc_shader_kind::shaderc_glsl_tess_evaluation_shader;
    break;
    }
    shaderc::SpvCompilationResult result = compiler.CompileGlslToSpv((const char*)buff, size, compileKind, (const char *)fileInfoStr);
    if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
        //handle errors
        tlogError("compile shader to Spir-v Error, %s", fileInfoStr);
        tlogError("%s",result.GetErrorMessage().c_str());
        abort();
    }
    std::vector<uint32_t> byteCode;
    byteCode.assign(result.cbegin(), result.cend());

    //reflection
    try{
    
    spirv_cross::Compiler  glsl(byteCode);
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
        m_setInfoMap[set].emplace_back(info);
	}
	for (auto &resource : resources.uniform_buffers)
	{
		unsigned set = glsl.get_decoration(resource.id, spv::DecorationDescriptorSet);
		unsigned binding = glsl.get_decoration(resource.id, spv::DecorationBinding);
        spirv_cross::SPIRType uniformBufferType      = glsl.get_type(resource.type_id);
        const std::string &varName      = glsl.get_name(resource.id);
        unsigned uniformBufferStructSize  = glsl.get_declared_struct_size(uniformBufferType);
        uint32_t member_count = uniformBufferType.member_types.size();
        glsl.get_member_name(resource.base_type_id, 0);
		printf("Uniform %s at set = %u, binding = %u, size %u name %s\n", resource.name.c_str(), set, binding,uniformBufferStructSize, varName.c_str());
        DeviceShaderVKLocationInfo info;
        info.set = set;
        info.binding = binding;
        info.type = DeviceShaderVKLocationType::Uniform;
        info.size = uniformBufferStructSize;
        if(type == DeviceShaderType::VertexShader){
            info.stageFlag |= VK_SHADER_STAGE_VERTEX_BIT;
        }
        else{
        
            info.stageFlag |= VK_SHADER_STAGE_FRAGMENT_BIT;
        }
        //uniform block each member
        for (int i = 0; i < member_count; i++)
        {
			BlockBufferMember ubm;
			ubm.name = glsl.get_member_name(resource.base_type_id, i);
			auto &member_type = glsl.get_type(uniformBufferType.member_types[i]);

			ubm.size = glsl.get_declared_struct_member_size(uniformBufferType, i);
			ubm.offset = glsl.type_struct_member_offset(uniformBufferType, i);
			info.m_member.push_back(ubm);
        }
        m_nameInfoMap[varName] = info;
        m_setInfoMap[set].emplace_back(info);
	}
    }
    catch(const spirv_cross::CompilerError &e){
    
        std::cout<<(e.what());
        abort();
    }


    //create Shader Module
    VkShaderModuleCreateInfo shaderCreateInfo = {};
    shaderCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderCreateInfo.codeSize = byteCode.size() * sizeof(uint32_t);
    shaderCreateInfo.pCode = &byteCode[0];
    VkShaderModule * shaderModule = (VkShaderModule *)malloc(sizeof(shaderModule));
    VkResult res = vkCreateShaderModule(VKRenderBackEnd::shared()->getDevice(), &shaderCreateInfo, NULL, shaderModule);
    if(res != 0)
    {
        printf("res fuck fuck %d",res);
        abort();
    
    }
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

DeviceShaderVKLocationInfo DeviceShaderVK::getLocationInfo(std::string name)
{
    auto result = m_nameInfoMap.find(name);
    if(result == m_nameInfoMap.end())
    {
        tlogError("criticalError%s", name);
        abort();
    }
    return result->second;
}

bool DeviceShaderVK::findLocationInfo(std::string name)
{
    return m_nameInfoMap.find(name)!= m_nameInfoMap.end();
}

std::unordered_map<std::string, DeviceShaderVKLocationInfo>& DeviceShaderVK::getNameLocationMap()
{
    return m_nameInfoMap;
}

void DeviceShaderVK::createDescriptorSetLayOut()
{
    m_descriptorSetLayout.resize(m_setInfoMap.size());

    for(auto&iter :m_setInfoMap){
        int setIndex = iter.first;
        auto shader = this;
        auto& locationMap = iter.second;//shader->getNameLocationMap();
        std::vector<VkDescriptorSetLayoutBinding> descriptorLayoutList;
        for(auto & locationInfo : locationMap){
            VkDescriptorSetLayoutBinding layOutBinding{};
            layOutBinding.binding = locationInfo.binding;
            if(locationInfo.type == DeviceShaderVKLocationType::Uniform)
            {
                layOutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            }
            else if(locationInfo.type == DeviceShaderVKLocationType::Sampler)
            {
                layOutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            }
        
            layOutBinding.descriptorCount = 1;
            layOutBinding.stageFlags = locationInfo.stageFlag;
            descriptorLayoutList.emplace_back(layOutBinding);
        }
        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = descriptorLayoutList.size();
        layoutInfo.pBindings = descriptorLayoutList.data();
        auto outDescripotr = m_descriptorSetLayout[setIndex];
        auto res = vkCreateDescriptorSetLayout(VKRenderBackEnd::shared()->getDevice(), &layoutInfo, nullptr, &outDescripotr);
        if(res != VK_SUCCESS){
        
            abort();
        }
        m_descriptorSetLayout[setIndex] = outDescripotr;
    }
}

VkDescriptorSetLayout& DeviceShaderVK::getDescriptorSetLayOut()
{
    return m_descriptorSetLayout[0];
}

VkDescriptorSetLayout& DeviceShaderVK::getMaterialDescriptorSetLayOut()
{
    return m_descriptorSetLayout[1];
}

bool DeviceShaderVK::isHaveMaterialDescriptorSetLayOut()
{
    return m_setInfoMap.find(1) != m_setInfoMap.end();
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
