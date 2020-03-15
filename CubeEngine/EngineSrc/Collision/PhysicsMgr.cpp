#include "PhysicsMgr.h"
#include "External/Bullet/btBulletDynamicsCommon.h"
#include "External/Bullet/LinearMath/btVector3.h"

#include "External/Bullet/btBulletDynamicsCommon.h"
#include "External/Bullet/LinearMath/btVector3.h"
#include "External/Bullet/LinearMath/btAlignedObjectArray.h"
#include "BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.h"
#include "BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h"
#include "btBulletDynamicsCommon.h"
#include <assert.h>
#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#include "Utility/log/Log.h"
#include <iostream>
#include "BulletCollision/NarrowPhaseCollision/btRaycastCallback.h"
#define dSINGLE

#define ARRAY_SIZE_Y 5
#define ARRAY_SIZE_X 5
#define ARRAY_SIZE_Z 5

namespace tzw
{
	TZW_SINGLETON_IMPL(PhysicsMgr)

PhysicsMgr::PhysicsMgr(): m_collisionConfiguration(nullptr), m_dispatcher(nullptr), m_broadphase(nullptr),
                          m_solver(nullptr), m_dynamicsWorld(nullptr)
{
}

btBoxShape* PhysicsMgr::createBoxShape(const btVector3& halfExtents)
	{
		btBoxShape* box = new btBoxShape(halfExtents);
		return box;
	}
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
			createRigidBodyInternal(mass, groundTransform, groundShape, btVector4(0, 0, 1, 1));
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

bool callbackFunc(btManifoldPoint& cp,const btCollisionObjectWrapper* obj1,int id1,int index1,const btCollisionObjectWrapper* obj2,int id2,int index2)
{
	//void* ID1 = obj1->m_collisionObject->getUserPointer();
	//void* ID2 = obj2->m_collisionObject->getUserPointer();
	//tlog("aaaa");
	return false;
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

		m_dynamicsWorld->getBroadphase()->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());

		
		m_dynamicsWorld->setGravity(btVector3(0, -10, 0));
		m_dynamicsWorld->setForceUpdateAllAabbs(true);
		//gContactAddedCallback = callbackFunc;
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
	// persistentCollisions map.
	static std::map<void *, std::vector<void *>> persistentCollisions;
	void PhysicsMgr::stepSimulation(float deltaTime)
	{
		if (m_dynamicsWorld)
		{
			m_dynamicsWorld->stepSimulation(deltaTime, 10, 1.0f / 120.0f);
			syncPhysicsToGraphics();
		    btDispatcher* dp = m_dynamicsWorld->getDispatcher();
		    const int numManifolds = dp->getNumManifolds();
			// New collision map
			std::map<void *, std::vector<void *>> newCollisions;
		    for ( int m=0; m<numManifolds; ++m )
		    {
	            btPersistentManifold* man = dp->getManifoldByIndexInternal( m );
	            const btRigidBody* obA = static_cast<const btRigidBody*>(man->getBody0());
	            const btRigidBody* obB = static_cast<const btRigidBody*>(man->getBody1());
	            const void* ptrA = obA->getUserPointer();
	            const void* ptrB = obB->getUserPointer();
		    	if(!ptrA) continue;
	            // use user pointers to determine if objects are eligible for destruction.
	            const int numc = man->getNumContacts();
	            float totalImpact = 0.0f;
	            for ( int c=0; c<numc; ++c )
	            {
					btManifoldPoint& pt = man->getContactPoint(c);
	                // If it is a new collision, add to the newCollision list
	               if (std::find(newCollisions[obA->getUserPointer()].begin(), newCollisions[obA->getUserPointer()].end(), obB->getUserPointer()) == newCollisions[obA->getUserPointer()].end()) 
	               {
						newCollisions[obA->getUserPointer()].emplace_back(obB->getUserPointer());
	               }
	            }
		    }
		    // Iterate over new collisions and add new collision to persistent collisions if it does not exist
		    std::map<void *, std::vector<void *>>::iterator newCollisionIterator = newCollisions.begin();
		    while (newCollisionIterator != newCollisions.end())
		    {
		        for (auto item : newCollisionIterator->second)
		        {
		            if (std::find(persistentCollisions[newCollisionIterator->first].begin(), persistentCollisions[newCollisionIterator->first].end(), item) == persistentCollisions[newCollisionIterator->first].end()) 
		            {
		                // We can play our collision audio here
		                persistentCollisions[newCollisionIterator->first].emplace_back(item);
		            	auto rig = static_cast<PhysicsListener *>(newCollisionIterator->first);
		            	if(rig->m_onHitCallBack)
		            	{
		            		rig->m_onHitCallBack(vec3());
		            	}
		            }
		        }
		        newCollisionIterator++;
		    }

		    // Iterate over persistent collisions and remove all collisions that did not exist in new collision
		    std::map<void *, std::vector<void *>>::iterator persistentCollisionIterator = persistentCollisions.begin();
		    while (persistentCollisionIterator != persistentCollisions.end())
		    {
		        std::vector<void *>::iterator iter;
		        for (iter = persistentCollisionIterator->second.begin(); iter != persistentCollisionIterator->second.end(); ) 
		        {
		            if (std::find(newCollisions[persistentCollisionIterator->first].begin(), newCollisions[persistentCollisionIterator->first].end(), *iter) != newCollisions[persistentCollisionIterator->first].end())
		            {
		                ++iter;
		            }
		            else
		            {
		                iter = persistentCollisionIterator->second.erase(iter);
		            }
		        }

		        persistentCollisionIterator++;
		    }
		}
	}

	vec3 PhysicsMgr::toV3(btVector3 v)
	{
		return vec3(v.getX(), v.getY(), v.getZ());
	}
	

	btVector3 PhysicsMgr::tobV3(vec3 v)
	{
		return btVector3(v.x, v.y, v.z);
	}


	PhysicsHitResult::PhysicsHitResult():isHit(false)
	{
	}

	btRigidBody* PhysicsMgr::createRigidBodyInternal(float mass, const btTransform& startTransform, btCollisionShape* shape, const btVector4& color = btVector4(1, 0, 0, 1))
	{
		// btAssert((!shape || shape->getShapeType() != INVALID_SHAPE_PROXYTYPE));

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
		return body;
	}

	btDiscreteDynamicsWorld* PhysicsMgr::getDynamicsWorld() const
	{
		return m_dynamicsWorld;
	}

	bool PhysicsMgr::rayCastCloset(vec3 from, vec3 to, PhysicsHitResult &result)
	{
		btVector3 from_v = btVector3(from.x, from.y, from.z);
		btVector3 to_v = btVector3(to.x, to.y, to.z);
		btCollisionWorld::ClosestRayResultCallback allResults(from_v, to_v);
		allResults.m_flags |= btTriangleRaycastCallback::kF_KeepUnflippedNormal;
		//kF_UseGjkConvexRaytest flag is now enabled by default, use the faster but more approximate algorithm
		//allResults.m_flags |= btTriangleRaycastCallback::kF_UseSubSimplexConvexCastRaytest;
		allResults.m_flags |= btTriangleRaycastCallback::kF_UseSubSimplexConvexCastRaytest;

		m_dynamicsWorld->rayTest(from_v, to_v, allResults);
		result.isHit = allResults.hasHit();
		result.posInWorld = toV3(allResults.m_hitPointWorld);
		result.normal = toV3(allResults.m_hitNormalWorld);
		if(allResults.hasHit())
		{
		
			return true;
		}
		return false;
	}

	void PhysicsMgr::syncPhysicsToGraphics()
	{
		int numCollisionObjects = m_dynamicsWorld->getNumCollisionObjects();
		{
			for (int i = 0; i<numCollisionObjects; i++)
			{
				//B3_PROFILE("writeSingleInstanceTransformToCPU");
				btCollisionObject* colObj = m_dynamicsWorld->getCollisionObjectArray()[i];

				int index = colObj->getUserIndex();
				void * ptr = colObj->getUserPointer();
				if (index >= 0 && !colObj->isStaticObject())
				{
					btVector3 pos = colObj->getWorldTransform().getOrigin();
					btQuaternion orn = colObj->getWorldTransform().getRotation();
					auto rig = static_cast<PhysicsListener *>(colObj->getUserPointer());
					vec3 posA = vec3(pos.x(), pos.y(), pos.z());
					Quaternion rot(orn.x(), orn.y(), orn.z(), orn.w());
					rig->recievePhysicsInfo(posA, rot);     
				}
			}
		}
	}

PhysicsRigidBody* PhysicsMgr::createRigidBody(float massValue, Matrix44& transform, AABB& aabb)
{
	auto rig = new PhysicsRigidBody();
	btScalar	mass(massValue);
	btTransform startTransform;
	startTransform.setIdentity();
	bool isDynamic = (mass != 0.f);
	vec3 half = aabb.half();
	btBoxShape* colShape = shared()->createBoxShape(btVector3(half.x, half.y, half.z));
	btVector3 localInertia(0, 0, 0);
	if (isDynamic)
	{
		colShape->calculateLocalInertia(mass, localInertia);
	}	
	startTransform.setFromOpenGLMatrix(transform.data());
	auto btRig = shared()->createRigidBodyInternal(mass, startTransform, colShape, btVector4(1, 0, 0, 1));
	rig->setRigidBody(btRig);
	rig->genUserIndex();
	btRig->setUserIndex(rig->userIndex());
	btRig->setUserPointer(static_cast<PhysicsListener * >(rig));
	return rig;
}

PhysicsRigidBody* PhysicsMgr::createRigidBodySphere(float massValue, Matrix44 transform, float radius)
{
	auto rig = new PhysicsRigidBody();
	btScalar	mass(massValue);
	btTransform startTransform;
	startTransform.setIdentity();
	bool isDynamic = (mass != 0.f);
	btSphereShape* colShape = new btSphereShape(radius);
	btVector3 localInertia(0, 0, 0);
	if (isDynamic)
	{
		colShape->calculateLocalInertia(mass, localInertia);
	}	
	startTransform.setFromOpenGLMatrix(transform.data());
	auto btRig = shared()->createRigidBodyInternal(mass, startTransform, colShape, btVector4(1, 0, 0, 1));
	rig->setRigidBody(btRig);
	rig->genUserIndex();
	btRig->setUserIndex(rig->userIndex());
	btRig->setUserPointer(static_cast<PhysicsListener * >(rig));

	return rig;
}

PhysicsRigidBody * PhysicsMgr::createRigidBodyCylinder(float massValue, float topRadius, float bottomRadius, float height, Matrix44 transform)
{
	auto rig = new PhysicsRigidBody();
	btScalar	mass(massValue);
	btTransform startTransform;
	startTransform.setIdentity();
	bool isDynamic = (mass != 0.f);
	btCylinderShape* colShape = new btCylinderShapeZ(btVector3(topRadius, bottomRadius, height * 0.5));
	btVector3 localInertia(0, 0, 0);
	if (isDynamic)
	{
		colShape->calculateLocalInertia(mass, localInertia);
	}	
	startTransform.setFromOpenGLMatrix(transform.data());
	auto btRig = shared()->createRigidBodyInternal(mass, startTransform, colShape, btVector4(1, 0, 0, 1));
	rig->setRigidBody(btRig);
	rig->genUserIndex();
	btRig->setUserIndex(rig->userIndex());
	btRig->setUserPointer(rig);
	return rig;
}


PhysicsRigidBody* PhysicsMgr::createRigidBodyMesh(Mesh* mesh, Matrix44* transform)
{
		auto rig = new PhysicsRigidBody();
		btScalar	mass(0.0f);
		btTransform startTransform;
		btTriangleMesh * meshInterface = new btTriangleMesh();
		auto idxSize = mesh->getIndicesSize();
		for(auto i =0; i < idxSize; i+=3)
		{
			auto v1 = mesh->getVertex(i);
			auto v2 = mesh->getVertex(i + 1);
			auto v3 = mesh->getVertex(i + 2);
			meshInterface->addTriangle(btVector3(v1.m_pos.x, v1.m_pos.y, v1.m_pos.z), btVector3(v2.m_pos.x, v2.m_pos.y, v2.m_pos.z), btVector3(v3.m_pos.x, v3.m_pos.y, v3.m_pos.z));
		}
		auto tetraShape = new btBvhTriangleMeshShape(meshInterface, true, true);

		startTransform.setIdentity();
		if (transform)
		{
			startTransform.setFromOpenGLMatrix(transform->data());
		}
		auto btRig = shared()->createRigidBodyInternal(mass, startTransform, tetraShape, btVector4(1, 0, 0, 1));
		btRig->setContactProcessingThreshold(BT_LARGE_FLOAT);
		btRig->setFriction (btScalar(0.9));
		btRig->setCcdMotionThreshold(.1);
		btRig->setCcdSweptSphereRadius(0.3);
		rig->setRigidBody(btRig);
		rig->genUserIndex();
		btRig->setUserIndex(rig->userIndex());
		btRig->setUserPointer(rig);
		return rig;
}

PhysicsRigidBody* PhysicsMgr::createRigidBodyFromCompund(float mass, Matrix44* transform, PhysicsCompoundShape * tzwShape)
{
	auto rig = new PhysicsRigidBody();
	bool isDynamic = (mass != 0.f);
	btVector3 localInertia(0, 0, 0);
	auto shape = tzwShape->getRawShape();
	//if (isDynamic)
	//	shape->calculateLocalInertia(mass, localInertia);
	btTransform startTransform;
	startTransform.setFromOpenGLMatrix(transform->data());
	auto btRig = shared()->createRigidBodyInternal(mass, startTransform, shape, btVector4(1, 0, 0, 1));
	rig->setRigidBody(btRig);
	rig->genUserIndex();
	btRig->setUserIndex(rig->userIndex());
	btRig->setUserPointer(rig);
	return rig;
}

	PhysicsHingeConstraint* PhysicsMgr::createHingeConstraint(PhysicsRigidBody* rbA, PhysicsRigidBody* rbB,
		Matrix44 frameInA, Matrix44 frameInB)
	{
		auto & rigA = *(rbA->rigidBody());
		auto & rigB = *(rbB->rigidBody());
		btTransform btFrameInA;
		btFrameInA.setFromOpenGLMatrix(frameInA.data());
		btTransform btFrameInB;
		btFrameInB.setFromOpenGLMatrix(frameInB.data());
		auto bthinge = new btHingeConstraint(rigA, rigB, btFrameInA, btFrameInB);
		auto hinge = new PhysicsHingeConstraint(bthinge);
		bthinge->setUserConstraintPtr(hinge);
		return hinge;
	}

	PhysicsHingeConstraint* PhysicsMgr::createHingeConstraint(PhysicsRigidBody * rbA, PhysicsRigidBody * rbB,
	                                                          const vec3& pivotInA, const vec3& pivotInB, const vec3& axisInA, const vec3& axisInB, bool useReferenceFrameA)
	{
		auto & rigA = *(rbA->rigidBody());
		auto & rigB = *(rbB->rigidBody());
		auto bthinge = new btHingeConstraint(rigA, rigB, btVector3(pivotInA.x, pivotInA.y, pivotInA.z), btVector3(pivotInB.x, pivotInB.y, pivotInB.z), btVector3(axisInA.x, axisInA.y, axisInA.z), btVector3(axisInB.x, axisInB.y, axisInB.z), useReferenceFrameA);
		auto hinge = new PhysicsHingeConstraint(bthinge);
		bthinge->setUserConstraintPtr(hinge);
		return hinge;
	}

	Physics6DofSpringConstraint* PhysicsMgr::create6DOFSprintConstraint(PhysicsRigidBody* rbA, PhysicsRigidBody* rbB,
		Matrix44 frameInA, Matrix44 frameInB)
	{
		auto & rigA = *(rbA->rigidBody());
		auto & rigB = *(rbB->rigidBody());
		btTransform btFrameInA;
		btFrameInA.setFromOpenGLMatrix(frameInA.data());
		btTransform btFrameInB;
		btFrameInB.setFromOpenGLMatrix(frameInB.data());
		auto bt6DOFSpring = new btGeneric6DofSpring2Constraint(rigA, rigB, btFrameInA, btFrameInB);
		auto spring6DOF = new Physics6DofSpringConstraint(bt6DOFSpring);
		return spring6DOF;
	}

	// Physics6DOFConstraint* PhysicsMgr::create6DOFConstraint(PhysicsRigidBody* rbA, PhysicsRigidBody* rbB,
	// 	Matrix44 frameInA, Matrix44 frameInB)
	// {
	// 	auto & rigA = *(rbA->rigidBody());
	// 	auto & rigB = *(rbB->rigidBody());
	// 	btTransform btFrameInA;
	// 	btFrameInA.setFromOpenGLMatrix(frameInA.data());
	// 	btTransform btFrameInB;
	// 	btFrameInB.setFromOpenGLMatrix(frameInB.data());
	// 	auto bt6DOFSpring = new btGeneric6DofConstraint(rigA, rigB, btFrameInA, btFrameInB, false);
	// 	auto spring6DOF = new Physics6DOFConstraint(bt6DOFSpring);
	// 	return spring6DOF;
	// }

	void PhysicsMgr::addRigidBody(PhysicsRigidBody* body) const
	{
		body->rigidBody()->activate();
		m_dynamicsWorld->addRigidBody(body->rigidBody());
	}

	void PhysicsMgr::removeRigidBody(PhysicsRigidBody* body) const
	{
		m_dynamicsWorld->removeRigidBody(body->rigidBody());
	}

	void PhysicsMgr::addConstraint(PhysicsConstraint* constraint, bool disableCollistion)
	{
		m_dynamicsWorld->addConstraint(constraint->constraint(), disableCollistion);
	}

	void PhysicsMgr::removeConstraint(PhysicsConstraint* constraint)
	{
		m_dynamicsWorld->removeConstraint(constraint->constraint());
	}
}
