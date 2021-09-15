#pragma once
#include "../DeviceShaderCollection.h"
#include "vulkan/vulkan.h"
#include <unordered_map>
#include "DeviceDescriptorSetLayoutVK.h"
#include "DeviceShaderVK.h"
#define MATERIAL_DESCRIPTOR_SET_ID (0)
#define OBJECT_DESCRIPTOR_SET_ID (1)
namespace tzw
{
class DeviceTextureVK;






class DeviceShaderCollectionVK : public DeviceShaderCollection
{
public:
	void addShader(const unsigned char * buff, size_t size, DeviceShaderType type, const unsigned char * fileInfoStr) override;
	bool create() override;
	bool finish() override;
	DeviceShaderVK * getVsModule();
	DeviceShaderVK * getFsModule();
	DeviceShaderVK * getCsModule();
	DeviceShaderVKLocationInfo getLocationInfo(std::string name);
	bool hasLocationInfo(std::string name);
	bool findLocationInfo(std::string name);
	std::unordered_map<std::string, DeviceShaderVKLocationInfo> & getNameLocationMap();
	void createDescriptorSetLayOut();
	VkDescriptorSetLayout getDescriptorSetLayOut();
	VkDescriptorSetLayout getMaterialDescriptorSetLayOut();
	bool isHavePerObjectDescriptorSetLayOut();
	std::unordered_map<int, std::vector<DeviceShaderVKLocationInfo>> & getSetInfo();
	DeviceDescriptorSetLayoutVK * getLayOutBySet(unsigned setID);
private:
	void mergeReflection(DeviceShader * shader);
	std::unordered_map<std::string, DeviceShaderVKLocationInfo> m_nameInfoMap;
	std::unordered_map<int, std::vector<DeviceShaderVKLocationInfo>> m_setInfoMap;
	DeviceShaderVK * m_vsShader;
	DeviceShaderVK * m_fsShader;
	DeviceShaderVK * m_tsShader;
	DeviceShaderVK * m_teShader;
	DeviceShaderVK * m_csShader;
	//std::vector<VkDescriptorSetLayout> m_descriptorSetLayout;
	std::vector<DeviceDescriptorSetLayoutVK * > m_descriptorSetLayouts;

};
};

