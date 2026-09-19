#pragma once
#include <string>
#include <cstdint>
#include <vector>
#include <unordered_map>
namespace tzw
{

enum class DeviceShaderType{
	VertexShader = 0,
	FragmentShader,
	TessControlShader,
	TessEvaulateShader,
	ComputeShader,

	UnkownShader,
};

constexpr uint32_t g_ShaderStageMask(DeviceShaderType type)
{
    return 1u << static_cast<uint32_t>(type);
}

enum class DeviceShaderBindingType
{
	Uniform,
	Sampler,
	StorageBuffer,
	StorageImage,
	
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



struct DeviceShaderBindingInfo{
	int set = 0;
	int binding = 0;
	DeviceShaderBindingType type = DeviceShaderBindingType::Uniform;
	uint32_t stageMask = 0;
	std::vector<BlockBufferMember> m_member;
	size_t size = 0;
	std::string name;
	int getBlockMemberIndex(std::string name);
	int arraySize = -1;
};

class DeviceShader
{
public:
	DeviceShader() = default;
	virtual ~DeviceShader() = default;
	virtual void compile(const unsigned char * buff, size_t size, DeviceShaderType type, const unsigned char * fileInfoStr) = 0;
	std::unordered_map<std::string, DeviceShaderBindingInfo> & getNameInfoMap() {return m_nameInfoMap;};
	std::unordered_map<int, std::vector<DeviceShaderBindingInfo>> & getSetInfoMap() {return m_setInfoMap;};
protected:
	std::unordered_map<std::string, DeviceShaderBindingInfo> m_nameInfoMap;
	std::unordered_map<int, std::vector<DeviceShaderBindingInfo>> m_setInfoMap;
	DeviceShaderType m_type {DeviceShaderType::UnkownShader};
};
}