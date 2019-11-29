#pragma once
#include "Math/vec3.h"
#include <vector>
#include "GamePartType.h"
namespace tzw
{
struct AttachmentInfo
{
	AttachmentInfo(vec3 p, vec3 n, vec3 u, std::string locale);
	vec3 pos;
	vec3 normal;
	vec3 up;
	std::string locale;
};
struct VisualInfo
{
	enum class VisualInfoType
	{
		CubePrimitive,
		CylinderPrimitive,
		Mesh,
		RightPrismPrimitive,
	};
	VisualInfoType type;
	vec3 size;
	std::string filePath;
	std::string diffusePath;
	std::string roughnessPath;
	std::string metallicPath;
	std::vector<std::string> extraFileList;
};

struct PhysicsInfo
{
	enum class PhysicsInfoType
	{
		BoxShape,
		CylinderShape,
	};
	PhysicsInfoType type;
	vec3 size;
	float mass;
};

struct GameItem
{
	bool isSpecialFunctionItem();
	GameItem();
	std::string m_name;
	GamePartType m_type;
	std::string m_desc;
	std::vector<AttachmentInfo> m_attachList;
	VisualInfo m_visualInfo;
	PhysicsInfo m_physicsInfo;
	int getTypeInInt();
};
}
