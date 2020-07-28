#pragma once
#include "../DeviceShader.h"
#include "vulkan/vulkan.h"
namespace tzw
{

class DeviceShaderVK : public DeviceShader
{
public:
	void addShader(const unsigned char * buff, size_t size, DeviceShaderType type, const unsigned char * fileInfoStr) override;
	bool create() override;
	bool finish() override;
private:
	VkShaderModule * m_vsShader;
	VkShaderModule * m_fsShader;
	VkShaderModule * m_tsShader;
	VkShaderModule * m_teShader;

};
};

