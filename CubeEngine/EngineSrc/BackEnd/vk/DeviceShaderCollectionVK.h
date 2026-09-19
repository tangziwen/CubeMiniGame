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
	~DeviceShaderCollectionVK() override;
	void addShader(const unsigned char * buff, size_t size, DeviceShaderType type, const unsigned char * fileInfoStr) override;
	bool create() override;
	bool finish() override;
	DeviceShaderVK * getVsModule();
	DeviceShaderVK * getFsModule();
	DeviceShaderVK * getCsModule();
	DeviceShaderBindingInfo getLocationInfo(std::string name);
	bool hasLocationInfo(std::string name);
	bool findLocationInfo(std::string name);
	std::unordered_map<std::string, DeviceShaderBindingInfo> & getNameLocationMap();
	void createDescriptorSetLayOut();
	VkDescriptorSetLayout getDescriptorSetLayOut();
	VkDescriptorSetLayout getMaterialDescriptorSetLayOut();
	bool isHavePerObjectDescriptorSetLayOut();
	std::unordered_map<int, std::vector<DeviceShaderBindingInfo>> & getSetInfo();
	DeviceDescriptorSetLayoutVK * getLayOutBySet(unsigned setID);
private:
	void mergeReflection(DeviceShader * shader);
	std::unordered_map<std::string, DeviceShaderBindingInfo> m_nameInfoMap;
	std::unordered_map<int, std::vector<DeviceShaderBindingInfo>> m_setInfoMap;
	DeviceShaderVK * m_vsShader = nullptr;
	DeviceShaderVK * m_fsShader = nullptr;
	DeviceShaderVK * m_tsShader = nullptr;
	DeviceShaderVK * m_teShader = nullptr;
	DeviceShaderVK * m_csShader = nullptr;
	//std::vector<VkDescriptorSetLayout> m_descriptorSetLayout;
	std::vector<DeviceDescriptorSetLayoutVK * > m_descriptorSetLayouts;

};
};

