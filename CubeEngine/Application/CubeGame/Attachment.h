#pragma once
#include "Interface/Drawable3D.h"
#include "Collision/PhysicsShape.h"
#include "Math/Ray.h"
#include "Base/GuidObj.h"
#include "rapidjson/document.h"

#define GAME_PART_BLOCK 0
#define GAME_PART_CYLINDER 1
#define GAME_PART_LIFT 2
#define GAME_PART_CONTROL 3
#define GAME_PART_NOT_VALID 999

namespace tzw
{
	class GamePart;
	struct Attachment : public GuidObj
	{
		vec3 m_pos;
		vec3 m_normal;
		vec3 m_up;
		Attachment(vec3 thePos, vec3 n, vec3 up, GamePart * parent);
		Attachment();
		void getAttachmentInfo(vec3 & pos, vec3 & N, vec3 & up);
		void getAttachmentInfoWorld(vec3 & pos, vec3 & N, vec3 & up);
		Matrix44 getAttachmentInfoMat44();
		GamePart * m_parent;
		//each attachment hase a small AABB to detect collision
		bool isHit(Ray rayIn, vec3& hitpointInLocal);
		int attachmentType;
		Attachment * m_connected;
		std::string m_connectedGUID;
		void breakConnection();
	};
}