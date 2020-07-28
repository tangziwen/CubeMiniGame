#include "DeviceShaderVK.h"
#include <cassert>
#include "../VkRenderBackEnd.h"
void tzw::DeviceShaderVK::addShader(const unsigned char* buff, size_t size, DeviceShaderType type, const unsigned char* fileInfoStr)
{
	auto pShaderCode = buff;//ReadBinaryFile(pFileName, codeSize);
    int codeSize = size;
    assert(pShaderCode);
  
    VkShaderModuleCreateInfo shaderCreateInfo = {};
    shaderCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderCreateInfo.codeSize = codeSize;
    shaderCreateInfo.pCode = (const uint32_t*)pShaderCode;
    
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

bool tzw::DeviceShaderVK::create()
{
    m_vsShader = nullptr;
    m_fsShader = nullptr;
    m_tsShader = nullptr;
    m_teShader = nullptr;
	return true;
}

bool tzw::DeviceShaderVK::finish()
{
	return true;
}
