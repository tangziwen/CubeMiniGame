#include "PhysicsCompoundShape.h"
#include <functional>

namespace tzw
{

PhysicsCompoundShape::PhysicsCompoundShape()
{
	m_shape = new btCompoundShape();
}

void PhysicsCompoundShape::addChildShape(Matrix44* mat, btCollisionShape* shape)
{
	btTransform transform;
	transform.setFromOpenGLMatrix(mat->data());
	getRawShape()->addChildShape(transform, shape);
}

void PhysicsCompoundShape::getChildShapeTransform(int index, float * data)
{
	getRawShape()->getChildTransform(index).getOpenGLMatrix(data);
}

Matrix44 PhysicsCompoundShape::adjustPrincipalAxis(std::function<float (int index)> getMassFunc)
{
	auto shape = getRawShape();
	btScalar * masses = (btScalar *)malloc(sizeof(btScalar) * shape->getNumChildShapes());
	for(int i = 0; i < shape->getNumChildShapes(); i++)
	{
		masses[i] = getMassFunc(i);
	}
	btTransform principal;
	btVector3 inertia;
	shape->calculatePrincipalAxisTransform(masses, principal, inertia);
	btCompoundShape* compound2 = new btCompoundShape();
	// recompute the shift to make sure the compound shape is re-aligned
	for (int i=0; i < shape->getNumChildShapes(); i++)
	{
		//这个在Bullet的CompoundBoxes.cpp demo里，乘的顺序是反的(m_shape->getChildTransform(i) * principal.inverse())，交换后效果才对
		//而且bullet 的论坛里，我看大家也都是这个顺序，而不是demo里的顺序，不知道是不是demo的bug
		compound2->addChildShape( principal.inverse() * shape->getChildTransform(i),
								 shape->getChildShape(i));
	}
	delete m_shape;
	free(masses);
	m_shape = compound2;
	Matrix44 returnResult;
	float oglMat4[16];
	principal.getOpenGLMatrix(oglMat4);
	returnResult.copyFromArray(oglMat4);
	return returnResult;
}
btCompoundShape* PhysicsCompoundShape::getRawShape()
{
	return static_cast<btCompoundShape*>(m_shape);
}
}
