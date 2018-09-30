#pragma once
#include "Interface/Drawable3D.h"
#include "Collision/PhysicsShape.h"

namespace tzw
{
	class BlockPart
	{
	public:
		BlockPart();
	public:
		Drawable3D* getNode() const;
		void setNode(Drawable3D* node);
		void cook();
		PhysicsShape * getShape();
	private:
		PhysicsShape * m_shape;
		Drawable3D * m_node;
	};
}
