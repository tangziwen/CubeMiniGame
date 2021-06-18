#pragma once
#include "GamePartType.h"
#include "Interface/Drawable3D.h"
#include "Collision/PhysicsShape.h"
#include "Math/Ray.h"
#include "Base/GuidObj.h"
#include "rapidjson/document.h"


namespace tzw
{
	class GamePart;
	struct Attachment : public GuidObj
	{
		vec3 m_pos;
		vec3 m_normal;
		vec3 m_up;
		AABB m_localBound;

		Attachment(vec3 thePos, vec3 n, vec3 up, GamePart * parent);
		Attachment();
		void getAttachmentInfo(vec3 & pos, vec3 & N, vec3 & up);
		void getAttachmentInfoWorld(vec3 & pos, vec3 & N, vec3 & up);
		Matrix44 getAttachmentMat();
		Matrix44 getAttachmentMatOutterWorld();
		Matrix44 getAttachmentMatWorld();
		const AABB & getLocalBound()
		{
			return m_localBound;
		}
		void generateLocalBound()
		{
			AABB collisionBox;
			collisionBox.setMax(vec3(m_collisionSize / 2.0, m_collisionSize / 2.0, 0.1));
			collisionBox.setMin(vec3(m_collisionSize / -2.0, m_collisionSize / -2.0, -0.1));
			m_localBound = collisionBox;
		}
		GamePart * m_parent;
		//each attachment hase a small AABB to detect collision
		bool isHit(Ray rayIn, vec3& hitpointInLocal);
		int attachmentType;
		Attachment * m_connected;
		std::string m_connectedGUID;
		std::string m_locale;
		void breakConnection();
		float m_degree;
		float m_collisionSize;
		~Attachment();
	};
}