#pragma once
#include "BulletCollision/CollisionShapes/btCompoundShape.h"
#include "Math/Matrix44.h"
#include "PhysicsShape.h"
class btCompoundShape;
class btCollisionShape;
namespace tzw
{
class AABB;


class PhysicsCompoundShape: public PhysicsShape
	{
	public:
		PhysicsCompoundShape();
		void addChildShape(Matrix44* mat, btCollisionShape * shape);
		void getChildShapeTransform(int index, float * data);
		Matrix44 adjustPrincipalAxis();
		btCompoundShape * getRawShape() override;
	};
}
