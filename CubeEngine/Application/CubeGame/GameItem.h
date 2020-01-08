#pragma once
#include "Math/vec3.h"
#include <vector>
#include "GamePartType.h"
#include "3D/Thumbnail.h"
namespace tzw
{
struct AttachmentInfo
{
	AttachmentInfo(vec3 p, vec3 n, vec3 u, std::string locale, float collisionBoxSize);
	vec3 pos;
	vec3 normal;
	vec3 up;
	std::string locale;
	float collisionSize;
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
	std::string normalMapPath;
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
	bool hasAttributePanel();
	GameItem();
	std::string m_name;
	GamePartType m_type;
	std::string m_desc;
	std::vector<AttachmentInfo> m_attachList;
	VisualInfo m_visualInfo;
	PhysicsInfo m_physicsInfo;
	int getTypeInInt();
	int getThumbNailTextureId();
	ThumbNail *m_thumbNail;
	Texture * m_texture;
};
}
