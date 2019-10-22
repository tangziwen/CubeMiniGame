#pragma once
#include "BulletCollision/CollisionShapes/btCompoundShape.h"
#include "Math/Matrix44.h"
#include "PhysicsShape.h"
#include <functional>
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
		Matrix44 adjustPrincipalAxis(std::function<float (int index)> getMassFunc);
		btCompoundShape * getRawShape() override;
	};
}
