#pragma once
#include "Base/Node.h"
#include "PhysicsListener.h"
#include "Math/AABB.h"
class btRigidBody;

namespace tzw
{
	class PhysicsShape;
	class Drawable3D;
	class PhysicsRigidBody : public PhysicsListener
	{
	public:
		enum class SyncPolicy
		{
			SyncAll,
			SyncPosOnly,
			SyncNothing,
		};
		PhysicsRigidBody();
		void attach(Drawable3D * obj);
		void recievePhysicsInfo(vec3 pos, Quaternion rot) override;
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
		void setVelocity(vec3 velocity);
		void applyCentralForce(vec3 force);
		void applyForce(vec3 force, vec3 localposition);
		void applyCentralImpulse(vec3 force);
		void applyImpulse(vec3 force, vec3 localposition);
		bool isInWorld();
		void setCcdSweptSphereRadius(float radius);
		void setCcdMotionThreshold(float threshold);
		AABB getAABBInWorld();
		SyncPolicy getSyncPolicy() const;
		void setSyncPolicy(const SyncPolicy syncPolicy);
	private:
		btRigidBody * m_rigidBody;
		Drawable3D * m_parent;
		unsigned m_userIndex;
		SyncPolicy m_syncPolicy;

	};


}