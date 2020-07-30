#pragma once
#include "../DeviceShader.h"
#include "vulkan/vulkan.h"
#include <unordered_map>
namespace tzw
{
enum class DeviceShaderVKLocationType
{
	Uniform,
	Sampler,
	
};
struct DeviceShaderVKLocationInfo{
int set;
int binding;
DeviceShaderVKLocationType type;
VkShaderStageFlags stageFlag;
};

class DeviceShaderVK : public DeviceShader
{
public:
	void addShader(const unsigned char * buff, size_t size, DeviceShaderType type, const unsigned char * fileInfoStr) override;
	bool create() override;
	bool finish() override;
	VkShaderModule * getVsModule();
	VkShaderModule * getFsModule();
	DeviceShaderVKLocationInfo getLocationInfo(std::string name);
	std::unordered_map<std::string, DeviceShaderVKLocationInfo> & getNameLocationMap();
	void createDescriptorSetLayOut();
	VkDescriptorSetLayout & getDescriptorSetLayOut();
private:
	std::unordered_map<std::string, DeviceShaderVKLocationInfo> m_nameInfoMap;
	VkShaderModule * m_vsShader;
	VkShaderModule * m_fsShader;
	VkShaderModule * m_tsShader;
	VkShaderModule * m_teShader;
	VkDescriptorSetLayout m_descriptorSetLayout;

};
};

