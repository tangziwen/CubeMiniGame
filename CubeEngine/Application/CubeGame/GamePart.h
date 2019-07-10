#pragma once
#include "Interface/Drawable3D.h"
#include "Collision/PhysicsShape.h"
#include "Math/Ray.h"

namespace tzw
{
	class GamePart;
	class BearPart;
	class Island;
	struct Attachment
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
		BearPart * m_bearPart;
	};
	class GamePart
	{
	public:
		virtual Drawable3D* getNode() const;
		virtual void setNode(Drawable3D* node);
		virtual PhysicsShape * getShape();
		virtual Attachment * findProperAttachPoint(Ray ray, vec3 &attachPosition, vec3 &Normal, vec3 & up);
		virtual void attachTo(Attachment * attach);
		virtual void attachToFromOtherIsland(Attachment * attach, BearPart * bearing);
		virtual Attachment * getFirstAttachment();
		Island * m_parent;
	protected:
		PhysicsShape * m_shape;
		Drawable3D * m_node;


	};

}