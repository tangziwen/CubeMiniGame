#pragma once
#include "BulletCollision/CollisionShapes/btCompoundShape.h"
#include "Math/Matrix44.h"
class btCompoundShape;
class btCollisionShape;
namespace tzw
{
class AABB;


class PhysicsCompoundShape
	{
	public:
		PhysicsCompoundShape();
		void addChildShape(Matrix44* mat, btCollisionShape * shape);
		void getChildShapeTransform(int index, float * data);
		btCompoundShape * getShape();
		void finish();
	private:
		btCompoundShape * m_shape;
	};
}
