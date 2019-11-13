#include "PhysicsRigidBody.h"
#include "Interface/Drawable3D.h"
#include "BulletDynamics/Dynamics/btRigidBody.h"
#include "PhysicsShape.h"

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

void PhysicsRigidBody::recievePhysicsInfo(vec3 pos, Quaternion rot)
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

void PhysicsRigidBody::setMass(float mass, vec3 localInertia)
{
	m_rigidBody->setMassProps(mass, btVector3(localInertia.x, localInertia.y, localInertia.z));
}

float PhysicsRigidBody::getMass() const
{
	return 1.0f  / m_rigidBody->getInvMass();
}

void PhysicsRigidBody::setCollisionShape(PhysicsShape * shape)
{
	m_rigidBody->setCollisionShape(shape->getRawShape());
}

void PhysicsRigidBody::setWorldTransform(Matrix44& transform)
{
	btTransform startTransform;
	startTransform.setIdentity();
	startTransform.setFromOpenGLMatrix(transform.data());
	m_rigidBody->setWorldTransform(startTransform);
	m_rigidBody->setInterpolationWorldTransform(startTransform);
}

void PhysicsRigidBody::updateInertiaTensor()
{
	m_rigidBody->updateInertiaTensor();
	
}

void PhysicsRigidBody::activate()
{
	m_rigidBody->activate();
}

void PhysicsRigidBody::clearAll()
{
	btVector3 zeroVector(0,0,0);
	m_rigidBody->clearForces();
	m_rigidBody->setLinearVelocity(zeroVector);
	m_rigidBody->setAngularVelocity(zeroVector);
}

void PhysicsRigidBody::setVelocity(vec3 velocity)
{
	m_rigidBody->setLinearVelocity(btVector3(velocity.x, velocity.y, velocity.z));
	
}

void PhysicsRigidBody::applyCentralForce(vec3 force)
{
	m_rigidBody->applyCentralForce(btVector3(force.x, force.y, force.z));
}

void PhysicsRigidBody::applyForce(vec3 force, vec3 localposition)
{
	m_rigidBody->applyForce(btVector3(force.x, force.y, force.z), btVector3(localposition.x, localposition.y, localposition.z));
}

void PhysicsRigidBody::applyCentralImpulse(vec3 force)
{
	m_rigidBody->applyCentralImpulse(btVector3(force.x, force.y, force.z));
}

void PhysicsRigidBody::applyImpulse(vec3 force, vec3 localposition)
{
	m_rigidBody->applyImpulse(btVector3(force.x, force.y, force.z), btVector3(localposition.x, localposition.y, localposition.z));
}

bool PhysicsRigidBody::isInWorld()
{
	return m_rigidBody->isInWorld();
}
}
