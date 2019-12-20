#ifndef TZW_PHYSIC_MGR_H
#define TZW_PHYSIC_MGR_H
#include "../Engine/EngineDef.h"
#include "External/Bullet/LinearMath/btAlignedObjectArray.h"
#include "PhysicsRigidBody.h"
#include "Math/Matrix44.h"
#include "Math/AABB.h"
#include "PhysicsHingeConstraint.h"
#include "PhysicsCompoundShape.h"
#include "Physics6DOFSprintConstraint.h"
// #include "Physics6DOFConstraint.h"
#define dSINGLE
class btRigidBody;
class btTransform;
class btCollisionShape;
class btVector4;
class btCollisionDispatcher;
class btDbvtBroadphase;
class btSequentialImpulseConstraintSolver;
class btBoxShape;
class btDiscreteDynamicsWorld;
class btDefaultCollisionConfiguration;
class btVector3;

namespace tzw
{
struct PhysicsHitResult
{
	PhysicsHitResult();

	bool isHit;
	vec3 posInWorld;
	vec3 normal;
};
class PhysicsMgr{
	public:
		TZW_SINGLETON_DECL(PhysicsMgr);
		PhysicsMgr();
		void start();
		void createEmptyDynamicsWorld();
		void stop();
		void createPlane(float x, float y, float z, float d);
		void createBox(float density, float width, float height, float depth);
		void stepSimulation(float delatime);
		vec3 toV3(btVector3 v);
		btVector3 tobV3(vec3 v);
		//
		void syncPhysicsToGraphics();
		PhysicsRigidBody * createRigidBody(float mass, Matrix44& transform, AABB& aabb);
		PhysicsRigidBody * createRigidBodySphere(float mass, Matrix44 transform, float radius);
		PhysicsRigidBody * createRigidBodyCylinder(float mass, float topRadius, float bottomRadius, float height,  Matrix44 transform);
		PhysicsRigidBody * createRigidBodyMesh(Mesh * obj, Matrix44* transform);
		PhysicsRigidBody * createRigidBodyFromCompund(float mass, Matrix44 * transform, PhysicsCompoundShape * shape);
		PhysicsHingeConstraint * createHingeConstraint(PhysicsRigidBody * rbA,PhysicsRigidBody * rbB, const vec3& pivotInA,const vec3& pivotInB, const vec3& axisInA,const vec3& axisInB, bool useReferenceFrameA = false);
		Physics6DofSpringConstraint * create6DOFSprintConstraint(PhysicsRigidBody * rbA,PhysicsRigidBody * rbB, Matrix44 frameInA, Matrix44 frameInB);
		// Physics6DOFConstraint * create6DOFConstraint(PhysicsRigidBody * rbA,PhysicsRigidBody * rbB, Matrix44 frameInA, Matrix44 frameInB);
		void addRigidBody(PhysicsRigidBody * body) const;
		void removeRigidBody(PhysicsRigidBody * body) const;

		void addConstraint(PhysicsConstraint * constraint, bool disableCollistion = false);
		void removeConstraint(PhysicsConstraint * constraint);
		btBoxShape* createBoxShape(const btVector3& halfExtents);
		btDiscreteDynamicsWorld* getDynamicsWorld() const;
		bool rayCastCloset(vec3 from, vec3 to, PhysicsHitResult &result);
	private:
		
		btRigidBody* createRigidBodyInternal(float mass, const btTransform& startTransform, btCollisionShape* shape, const btVector4& color);
		btDefaultCollisionConfiguration * m_collisionConfiguration;
		btCollisionDispatcher* m_dispatcher;
		btDbvtBroadphase* m_broadphase;
		btSequentialImpulseConstraintSolver* m_solver;
		btDiscreteDynamicsWorld* m_dynamicsWorld;

	private:
		btAlignedObjectArray<btCollisionShape*>	m_collisionShapes;
	};

}
#endif // !TZW_PHYSIC_MGR_H
