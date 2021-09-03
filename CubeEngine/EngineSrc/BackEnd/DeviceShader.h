#pragma once
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
	uint32_t stageFlag;
	std::vector<BlockBufferMember> m_member;
	size_t size;
	std::string name;
	int getBlockMemberIndex(std::string name);
};

class DeviceShader
{
public:
	DeviceShader() = default;
	virtual void compile(const unsigned char * buff, size_t size, DeviceShaderType type, const unsigned char * fileInfoStr) = 0;
	std::unordered_map<std::string, DeviceShaderVKLocationInfo> & getNameInfoMap() {return m_nameInfoMap;};
	std::unordered_map<int, std::vector<DeviceShaderVKLocationInfo>> & getSetInfoMap() {return m_setInfoMap;};
protected:
	std::unordered_map<std::string, DeviceShaderVKLocationInfo> m_nameInfoMap;
	std::unordered_map<int, std::vector<DeviceShaderVKLocationInfo>> m_setInfoMap;
	DeviceShaderType m_type {DeviceShaderType::UnkownShader};
};
}