#pragma once
#include "../DeviceShader.h"
#include "vulkan/vulkan.h"
#include <unordered_map>
#include "DeviceDescriptorSetLayoutVK.h"
#define MATERIAL_DESCRIPTOR_SET_ID (0)
#define OBJECT_DESCRIPTOR_SET_ID (1)
namespace tzw
{
class DeviceTextureVK;
enum class DeviceShaderVKLocationType
{
	Uniform,
	Sampler,
	
};

enum BaseDataType
{
	None,
	Boolean,
	Int,
	Half,
	Float,
	Double,
	IntArray,
	FloatArray,
	Vector2,
	Vector3,
	Vector4,
	Matrix3,
	Matrix4
};
struct BlockBufferMember
{
	BaseDataType type;
	std::string name;
	uint32_t size;
	uint32_t offset;
};

struct DeviceShaderVKLocationInfo{
	int set;
	int binding;
	DeviceShaderVKLocationType type;
	VkShaderStageFlags stageFlag;
	std::vector<BlockBufferMember> m_member;
	size_t size;
	std::string name;
	int getBlockMemberIndex(std::string name);
};

class DeviceShaderVK : public DeviceShader
{
public:
	void addShader(const unsigned char * buff, size_t size, DeviceShaderType type, const unsigned char * fileInfoStr) override;
	bool create() override;
	bool finish() override;
	VkShaderModule getVsModule();
	VkShaderModule getFsModule();
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
	std::unordered_map<std::string, DeviceShaderVKLocationInfo> m_nameInfoMap;
	std::unordered_map<int, std::vector<DeviceShaderVKLocationInfo>> m_setInfoMap;
	VkShaderModule m_vsShader;
	VkShaderModule m_fsShader;
	VkShaderModule m_tsShader;
	VkShaderModule m_teShader;
	//std::vector<VkDescriptorSetLayout> m_descriptorSetLayout;
	std::vector<DeviceDescriptorSetLayoutVK * > m_descriptorSetLayouts;

};
};

