#pragma once
#include "../DeviceShader.h"
#include "vulkan/vulkan.h"
namespace tzw
{
	class DeviceShaderVK: public DeviceShader
	{
	public:
		void compile(const unsigned char * buff, size_t size, DeviceShaderType type, const unsigned char * fileInfoStr) override;
		VkShaderModule getRawModule() { return m_rawModule;}
	protected:
		VkShaderModule m_rawModule;
	};

}