#include "BlockPart.h"
#include "3D/Primitive/CubePrimitive.h"
#include "Scene/SceneMgr.h"
#include "Collision/PhysicsMgr.h"

namespace tzw
{
BlockPart::BlockPart()
{
	m_node = new CubePrimitive(0.5, 0.5, 0.5);
	m_shape = new PhysicsShape();
	m_shape->initBoxShape(vec3(0.5, 0.5, 0.5));
}

Drawable3D* BlockPart::getNode() const
{
	return m_node;
}

void BlockPart::setNode(Drawable3D* node)
{
	m_node = node;
}

void BlockPart::cook()
{
	auto mat2 = m_node->getTranslationMatrix();
	auto aabb = m_node->getAABB();
	auto rigChasis = PhysicsMgr::shared()->createRigidBody(1.0, mat2, aabb);
	rigChasis->attach(m_node);
}

PhysicsShape* BlockPart::getShape()
{
	return m_shape;
}
}
