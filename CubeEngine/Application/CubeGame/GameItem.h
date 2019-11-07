#pragma once
#include "Math/vec3.h"
#include <vector>
#include "GamePartType.h"
namespace tzw
{
struct AttachmentInfo
{
	AttachmentInfo(vec3 p, vec3 n, vec3 u);
	vec3 pos;
	vec3 normal;
	vec3 up;
};
struct VisualInfo
{
	enum class VisualInfoType
	{
		CubePrimitive,
		CylinderPrimitive,
		Mesh,
	};
	VisualInfoType type;
	vec3 size;
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
};

struct GameItem
{
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
