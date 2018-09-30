#include "PhysicsShape.h"
#include "BulletCollision/CollisionShapes/btBoxShape.h"
#include "BulletCollision/CollisionShapes/btSphereShape.h"
#include "Math/AABB.h"
#include "BulletCollision/CollisionShapes/btCompoundShape.h"

namespace tzw
{
PhysicsShape::PhysicsShape()
{
	m_shape = nullptr;
}

void PhysicsShape::initBoxShape(AABB aabb)
{
	auto half = aabb.half();
	m_shape = new btBoxShape(btVector3(half.x, half.y, half.z));
}

void PhysicsShape::initBoxShape(vec3 size)
{
	m_shape = new btBoxShape(btVector3(size.x / 2.0f, size.y / 2.0f, size.z / 2.0f));
}

void PhysicsShape::initSphereShape(float radius)
{
	m_shape = new btSphereShape(radius);
}

btCollisionShape* PhysicsShape::getRawShape()
{
	return m_shape;
}
}
