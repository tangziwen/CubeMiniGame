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
