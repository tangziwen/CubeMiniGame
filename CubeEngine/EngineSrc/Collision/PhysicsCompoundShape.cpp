#include "PhysicsCompoundShape.h"
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
	m_shape->addChildShape(transform, shape);
}

btCompoundShape* PhysicsCompoundShape::getShape()
{
	return m_shape;
}

void PhysicsCompoundShape::finish()
{
	btScalar * masses = (btScalar *)malloc(sizeof(btScalar) * m_shape->getNumChildShapes());
	for(int i = 0; i < m_shape->getNumChildShapes(); i++)
	{
		masses[i] = 1.0;
	}
	btTransform principal;
	btVector3 inertia;
	m_shape->calculatePrincipalAxisTransform(masses, principal, inertia);
	btCompoundShape* compound2 = new btCompoundShape();
	// recompute the shift to make sure the compound shape is re-aligned
	for (int i=0; i < m_shape->getNumChildShapes(); i++)
	{
		compound2->addChildShape(m_shape->getChildTransform(i) * principal.inverse(),
								 m_shape->getChildShape(i));
	}
	delete m_shape;
	free(masses);
	m_shape = compound2;
}
}
