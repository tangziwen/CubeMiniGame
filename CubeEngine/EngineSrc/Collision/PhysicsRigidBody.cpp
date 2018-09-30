#include "PhysicsRigidBody.h"
#include "Interface/Drawable3D.h"
#include "BulletDynamics/Dynamics/btRigidBody.h"

namespace tzw
{
static int g_index = 0;
PhysicsRigidBody::PhysicsRigidBody()
{
	m_rigidBody = nullptr;
	m_parent = nullptr;
	m_userIndex = 0;
}

void PhysicsRigidBody::attach(Drawable3D* obj)
{
	m_parent = obj;
}

void PhysicsRigidBody::sync(vec3 pos, Quaternion rot)
{
	if(m_parent)
	{
		m_parent->setPos(pos);
		m_parent->setRotateQ(rot);
	}
}

btRigidBody* PhysicsRigidBody::rigidBody() const
{
	return m_rigidBody;
}

void PhysicsRigidBody::setRigidBody(btRigidBody* rigid_body)
{
	m_rigidBody = rigid_body;
}

Drawable3D* PhysicsRigidBody::parent() const
{
	return m_parent;
}

void PhysicsRigidBody::setParent(Drawable3D* parent)
{
	m_parent = parent;
}

unsigned PhysicsRigidBody::userIndex() const
{
	return m_userIndex;
}

void PhysicsRigidBody::setUserIndex(unsigned user_index)
{
	m_userIndex = user_index;
}

void PhysicsRigidBody::genUserIndex()
{
	g_index++;
	m_userIndex = g_index;
}

void PhysicsRigidBody::applyTorqueLocal(vec3 torqueV)
{
	btVector3 torqueWorld = m_rigidBody->getWorldTransform().getBasis()*btVector3(torqueV.x, torqueV.y, torqueV.z);
	m_rigidBody->applyTorque(torqueWorld);
}

void PhysicsRigidBody::applyTorqueImpulse(vec3 torqueV)
{
	btVector3 torqueWorld = m_rigidBody->getWorldTransform().getBasis()*btVector3(torqueV.x, torqueV.y, torqueV.z);
	m_rigidBody->applyTorqueImpulse(torqueWorld);
}

void PhysicsRigidBody::setFriction(float friction)
{
	m_rigidBody->setFriction(friction);
}

float PhysicsRigidBody::getFriction()
{
	return m_rigidBody->getFriction();
}

void PhysicsRigidBody::setRollingFriction(float rollingFriction)
{
	m_rigidBody->setRollingFriction(rollingFriction);
}

float PhysicsRigidBody::getRollingFriction()
{
	return m_rigidBody->getRollingFriction();
}
}
