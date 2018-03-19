#ifndef TZW_PHYSIC_MGR_H
#define TZW_PHYSIC_MGR_H
#include "../Engine/EngineDef.h"
#include "External/Bullet/btBulletDynamicsCommon.h"
#include "External/Bullet/LinearMath/btVector3.h"
#include "External/Bullet/LinearMath/btAlignedObjectArray.h"
namespace tzw
{
	class PhysicsMgr{
	public:
		TZW_SINGLETON_DECL(PhysicsMgr);
		void start();
		void createEmptyDynamicsWorld();
		void stop();
		void createPlane(float x, float y, float z, float d);
		void createBox(float density, float width, float height, float depth);
		void stepSimulation(float delatime);
		btBoxShape* createBoxShape(const btVector3& halfExtents);
		btRigidBody* createRigidBody(float mass, const btTransform& startTransform, btCollisionShape* shape, const btVector4& color);
		void syncPhysicsToGraphics();
	private:
		btDefaultCollisionConfiguration * m_collisionConfiguration;
		btCollisionDispatcher* m_dispatcher;
		btDbvtBroadphase* m_broadphase;
		btSequentialImpulseConstraintSolver* m_solver;
		btDiscreteDynamicsWorld* m_dynamicsWorld;
		btAlignedObjectArray<btCollisionShape*>	m_collisionShapes;
		
	};

}
#endif // !TZW_PHYSIC_MGR_H
