#include "PhysicsMgr.h"
#include "External/Bullet/btBulletDynamicsCommon.h"
#include "External/Bullet/LinearMath/btVector3.h"
#include "External/Bullet/LinearMath/btAlignedObjectArray.h"
#include <assert.h>
#define dSINGLE

#define ARRAY_SIZE_Y 5
#define ARRAY_SIZE_X 5
#define ARRAY_SIZE_Z 5

namespace tzw
{
	TZW_SINGLETON_IMPL(PhysicsMgr);
	void PhysicsMgr::start()
	{
		createEmptyDynamicsWorld();

		///create a few basic rigid bodies
		btBoxShape* groundShape = createBoxShape(btVector3(btScalar(50.), btScalar(50.), btScalar(50.)));

		m_collisionShapes.push_back(groundShape);

		btTransform groundTransform;
		groundTransform.setIdentity();
		groundTransform.setOrigin(btVector3(0, -50, 0));
		{
			btScalar mass(0.);
			createRigidBody(mass, groundTransform, groundShape, btVector4(0, 0, 1, 1));
		}
		{
			//create a few dynamic rigidbodies
			// Re-using the same collision is better for memory usage and performance

			btBoxShape* colShape = createBoxShape(btVector3(.1, .1, .1));


			//btCollisionShape* colShape = new btSphereShape(btScalar(1.));
			m_collisionShapes.push_back(colShape);

			/// Create Dynamic Objects
			btTransform startTransform;
			startTransform.setIdentity();

			btScalar	mass(1.f);

			//rigidbody is dynamic if and only if mass is non zero, otherwise static
			bool isDynamic = (mass != 0.f);

			btVector3 localInertia(0, 0, 0);
			if (isDynamic)
				colShape->calculateLocalInertia(mass, localInertia);

			//int index = 0;
			//for (int k = 0; k < ARRAY_SIZE_Y; k++)
			//{
			//	for (int i = 0; i < ARRAY_SIZE_X; i++)
			//	{
			//		for (int j = 0; j < ARRAY_SIZE_Z; j++)
			//		{
			//			startTransform.setOrigin(btVector3(
			//				btScalar(0.2*i),
			//				btScalar(2 + .2*k),
			//				btScalar(0.2*j)));


			//			auto body = createRigidBody(mass, startTransform, colShape, btVector4(1, 0, 0, 1));
			//			body->setUserIndex(index);
			//			index++;
			//		}
			//	}
			//}
		}
	}

	void PhysicsMgr::createEmptyDynamicsWorld()
	{
		///collision configuration contains default setup for memory, collision setup
		m_collisionConfiguration = new btDefaultCollisionConfiguration();
		//m_collisionConfiguration->setConvexConvexMultipointIterations();

		///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
		m_dispatcher = new	btCollisionDispatcher(m_collisionConfiguration);

		m_broadphase = new btDbvtBroadphase();

		///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
		btSequentialImpulseConstraintSolver* sol = new btSequentialImpulseConstraintSolver;
		m_solver = sol;

		m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher, m_broadphase, m_solver, m_collisionConfiguration);

		m_dynamicsWorld->setGravity(btVector3(0, -10, 0));
	}

	void PhysicsMgr::stop()
	{

	}

	void PhysicsMgr::createPlane(float x, float y, float z, float d)
	{
	}

	void PhysicsMgr::createBox(float density, float width, float height, float depth)
	{
	}

	void PhysicsMgr::stepSimulation(float deltaTime)
	{
		if (m_dynamicsWorld)
		{
			m_dynamicsWorld->stepSimulation(deltaTime);
			syncPhysicsToGraphics();
		}
		
	}

	btBoxShape* PhysicsMgr::createBoxShape(const btVector3& halfExtents)
	{
		btBoxShape* box = new btBoxShape(halfExtents);
		return box;
	}


	btRigidBody* PhysicsMgr::createRigidBody(float mass, const btTransform& startTransform, btCollisionShape* shape, const btVector4& color = btVector4(1, 0, 0, 1))
	{
		btAssert((!shape || shape->getShapeType() != INVALID_SHAPE_PROXYTYPE));

		//rigidbody is dynamic if and only if mass is non zero, otherwise static
		bool isDynamic = (mass != 0.f);

		btVector3 localInertia(0, 0, 0);
		if (isDynamic)
			shape->calculateLocalInertia(mass, localInertia);

		//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects

#define USE_MOTIONSTATE 1
#ifdef USE_MOTIONSTATE
		btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);

		auto cInfo = new btRigidBody::btRigidBodyConstructionInfo(mass, myMotionState, shape, localInertia);
		cInfo->m_collisionShape = NULL;
		cInfo->m_collisionShape = shape;
		btRigidBody* body = new btRigidBody(*cInfo);
		//body->setContactProcessingThreshold(m_defaultContactProcessingThreshold);

#else
		btRigidBody* body = new btRigidBody(mass, 0, shape, localInertia);
		body->setWorldTransform(startTransform);
#endif//

		body->setUserIndex(-1);
		m_dynamicsWorld->addRigidBody(body);
		return body;
	}

	void PhysicsMgr::syncPhysicsToGraphics()
	{
		int numCollisionObjects = m_dynamicsWorld->getNumCollisionObjects();
		{
			for (int i = 0; i<numCollisionObjects; i++)
			{
				//B3_PROFILE("writeSingleInstanceTransformToCPU");
				btCollisionObject* colObj = m_dynamicsWorld->getCollisionObjectArray()[i];
				btVector3 pos = colObj->getWorldTransform().getOrigin();
				btQuaternion orn = colObj->getWorldTransform().getRotation();
				int index = colObj->getUserIndex();
				if (index >= 0)
				{
					//m_data->m_glApp->m_renderer->writeSingleInstanceTransformToCPU(pos, orn, index);
				}
			}
		}
	}
}