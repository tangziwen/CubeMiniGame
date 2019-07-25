#pragma once
#include "Base/Node.h"
class btRigidBody;

namespace tzw
{
	class PhysicsShape;
	class Drawable3D;
	class PhysicsRigidBody
	{
	public:
		PhysicsRigidBody();
		void attach(Drawable3D * obj);
		void sync(vec3 pos, Quaternion rot);
		btRigidBody* rigidBody() const;
		void setRigidBody(btRigidBody* rigid_body);
		Drawable3D* parent() const;
		void setParent(Drawable3D* parent);
		unsigned userIndex() const;
		void setUserIndex(unsigned user_index);
		void genUserIndex();
		void applyTorqueLocal(vec3 torqueV);
		void applyTorqueImpulse(vec3 torqueV);
		void setFriction(float friction);
		float getFriction();
		void setRollingFriction(float rollingFriction);
		float getRollingFriction();
		void setMass(float mass, vec3 localInertia);
		float getMass() const;
		void setCollisionShape(PhysicsShape * shape);
		void setWorldTransform(Matrix44& transform);
		void updateInertiaTensor();
		void activate();
		void clearAll();
	private:
		btRigidBody * m_rigidBody;
		Drawable3D * m_parent;
		unsigned m_userIndex;
	};


}