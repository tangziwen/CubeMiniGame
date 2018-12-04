#pragma once
#include "Interface/Drawable3D.h"
#include "Collision/PhysicsShape.h"
#include "Math/Ray.h"
namespace tzw
{
	class GamePart
	{
	public:
		virtual Drawable3D* getNode() const;
		virtual void setNode(Drawable3D* node);
		virtual PhysicsShape * getShape();
		virtual bool findProperAttachPoint(Ray ray, vec3 &attachPosition, vec3 &Normal, vec3 & up);
		virtual void attachTo(vec3 attachPosition, vec3 Normal, vec3 up);
	protected:
		PhysicsShape * m_shape;
		Drawable3D * m_node;


	};

}