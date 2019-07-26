#include "PhysicsMgr.h"
#include "External/Bullet/btBulletDynamicsCommon.h"
#include "External/Bullet/LinearMath/btVector3.h"

#include "External/Bullet/btBulletDynamicsCommon.h"
#include "External/Bullet/LinearMath/btVector3.h"
#include "External/Bullet/LinearMath/btAlignedObjectArray.h"
#include "BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.h"
#include "BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h"
#include <assert.h>
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
		m_dynamicsWorld->setForceUpdateAllAabbs(true);
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




	btRigidBody* PhysicsMgr::createRigidBodyInternal(float mass, const btTransform& startTransform, btCollisionShape* shape, const btVector4& color = btVector4(1, 0, 0, 1))
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
				if (index >= 0 && !colObj->isStaticObject())
				{
					auto rig = (PhysicsRigidBody *)colObj->getUserPointer();
					vec3 posA = vec3(pos.x(), pos.y(), pos.z());
					Quaternion rot(orn.x(), orn.y(), orn.z(), orn.w());
					rig->sync(posA, rot);
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
	btRig->setUserPointer(rig);

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
	btRig->setUserPointer(rig);

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
		btRig->setCcdSweptSphereRadius(0);
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
	if (isDynamic)
		shape->calculateLocalInertia(mass, localInertia);
	btTransform startTransform;
	startTransform.setFromOpenGLMatrix(transform->data());
	auto btRig = shared()->createRigidBodyInternal(mass, startTransform, shape, btVector4(1, 0, 0, 1));
	rig->setRigidBody(btRig);
	rig->genUserIndex();
	btRig->setUserIndex(rig->userIndex());
	btRig->setUserPointer(rig);
	return rig;
}

	PhysicsHingeConstraint* PhysicsMgr::createHingeConstraint(PhysicsRigidBody * rbA, PhysicsRigidBody * rbB,
		const vec3& pivotInA, const vec3& pivotInB, const vec3& axisInA, const vec3& axisInB, bool useReferenceFrameA)
	{
		auto & rigA = *(rbA->rigidBody());
		auto & rigB = *(rbB->rigidBody());
		auto bthinge = new btHingeConstraint(rigA, rigB, btVector3(pivotInA.x, pivotInA.y, pivotInA.z), btVector3(pivotInB.x, pivotInB.y, pivotInB.z), btVector3(axisInA.x, axisInA.y, axisInA.z), btVector3(axisInB.x, axisInB.y, axisInB.z), useReferenceFrameA);
		auto hinge = new PhysicsHingeConstraint(bthinge);
		bthinge->setUserConstraintPtr(hinge);
		m_dynamicsWorld->addConstraint(bthinge);
		return hinge;
	}

	Physics6DofConstraint* PhysicsMgr::create6DOFSprintConstraint(PhysicsRigidBody* rbA, PhysicsRigidBody* rbB,
		Matrix44 frameInA, Matrix44 frameInB)
	{
		auto & rigA = *(rbA->rigidBody());
		auto & rigB = *(rbB->rigidBody());
		btTransform btFrameInA;
		btFrameInA.setFromOpenGLMatrix(frameInA.data());
		btTransform btFrameInB;
		btFrameInB.setFromOpenGLMatrix(frameInB.data());
		auto bt6DOFSpring = new btGeneric6DofSpringConstraint(rigA, rigB, btFrameInA, btFrameInB, true);
		auto spring6DOF = new Physics6DofConstraint(bt6DOFSpring);
		m_dynamicsWorld->addConstraint(bt6DOFSpring);
		return spring6DOF;
	}

	void PhysicsMgr::addRigidBody(PhysicsRigidBody* body) const
	{
		body->rigidBody()->activate();
		m_dynamicsWorld->addRigidBody(body->rigidBody());
	}

	void PhysicsMgr::removeRigidBody(PhysicsRigidBody* body) const
	{
		m_dynamicsWorld->removeRigidBody(body->rigidBody());
	}

	void PhysicsMgr::addConstraint(PhysicsHingeConstraint* constraint)
	{
		m_dynamicsWorld->addConstraint(constraint->constraint());
	}

	void PhysicsMgr::removeConstraint(PhysicsHingeConstraint* constraint)
	{
		m_dynamicsWorld->removeConstraint(constraint->constraint());
	}
}
