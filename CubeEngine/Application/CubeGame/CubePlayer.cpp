#include "CubePlayer.h"
#include "EngineSrc/Scene/SceneMgr.h"
#include "EngineSrc/3D/Model/Model.h"
#include "GameConfig.h"
#include "GameWorld.h"
#include "EngineSrc/Event/EventMgr.h"
#include <iostream>
#include "3D/Primitive/CubePrimitive.h"
#include "3D/Primitive/SpherePrimitive.h"
#include "3D/Sky.h"
#include "Projectile.h"

#include "3D/Vegetation/Grass.h"
#include "EngineSrc/Collision/PhysicsMgr.h"
#include "BuildingSystem.h"
#include "3D/Primitive/CylinderPrimitive.h"

namespace tzw
{

	CubePlayer::CubePlayer(Node* mainRoot)
	{
		m_currMode = Mode::MODE_DEFORM_SPHERE;

		FPSCamera* camera = FPSCamera::create(g_GetCurrScene()->defaultCamera());
		camera->setPos(vec3(5, 50, -5));
		mainRoot->addChild(camera);
		g_GetCurrScene()->setDefaultCamera(camera);
		m_camera = camera;
		camera->reCache();
		m_camera->setIsEnableGravity(true);


		auto NUM_MESHES = 5;
        for (int i = 0; i < NUM_MESHES ; i++) {
			auto testModel = Model::create("Models/phoenix_ugv.tzw");
			testModel->setScale(vec3(0.2, 0.2, 0.2));
			testModel->setPos(vec3(-15.0f, 30.0f, 3.0f + i * 30.0f));
			g_GetCurrScene()->addNode(testModel);
        }          

		auto testWall = Model::create("Models/quad.tzw");
		testWall->setPos(vec3(0, 0, 90));
		testWall->setRotateE(90, 0, 0);
		testWall->setScale(50, 100, 100);
		g_GetCurrScene()->addNode(testWall);

		//m_camera->addChild(m_gunModel);

		auto pos = getPos();
		oldPosX = pos.x / ((MAX_BLOCK + 1) * BLOCK_SIZE);
		oldPosZ = (-1.0f * pos.z) / ((MAX_BLOCK + 1) * BLOCK_SIZE);
		Sky::shared()->setCamera(m_camera);
		EventMgr::shared()->addFixedPiorityListener(this);

		m_enableGravity = true;
	}

	FPSCamera* CubePlayer::camera() const
	{
		return m_camera;
	}

	void CubePlayer::setCamera(FPSCamera* camera)
	{
		m_camera = camera;
	}

	vec3 CubePlayer::getPos()
	{
		return m_camera->getPos();
	}

	void CubePlayer::logicUpdate(float dt)
	{
		static float theTime = 0.0f;
		//vec3 oldPos = m_gunModel->getPos();
		float offset = 0.002;
		float freq = 1;
		if (m_camera->getIsMoving())
		{
			offset = 0.006;
			freq = 6;
		}
		theTime += freq * dt;
		// m_gunModel->setPos(vec3(oldPos.x, -0.2 + sinf(theTime) * offset, oldPos.z));
		if (checkIsNeedUpdateChunk())
		{
			GameWorld::shared()->loadChunksAroundPlayer();
		}
	}

	bool CubePlayer::checkIsNeedUpdateChunk()
	{
		auto pos = getPos();
		int posX = pos.x / ((MAX_BLOCK + 1) * BLOCK_SIZE);
		int posZ = (-1.0f * pos.z) / ((MAX_BLOCK + 1) * BLOCK_SIZE);
		if (posX != oldPosX || posZ != oldPosZ)
		{
			oldPosX = posX;
			oldPosZ = posZ;
			return true;
		}
		return false;
	}
	static PhysicsRigidBody *wheelFrontLeft = nullptr;
	static PhysicsRigidBody *wheelFrontRight = nullptr;

	static PhysicsHingeConstraint * constraint1 = nullptr;
	static PhysicsHingeConstraint * constraint2 = nullptr;
	bool CubePlayer::onKeyPress(int keyCode)
	{
		switch (keyCode)
		{
		case TZW_KEY_4:
			break;
		case TZW_KEY_T:
			{
				constraint1->enableAngularMotor(true, -10, 100);
				constraint2->enableAngularMotor(true, -10, 100);
			}
			break;
		}
		return false;
	}

	bool CubePlayer::onKeyRelease(int keyCode)
	{
		switch (keyCode)
		{
		case TZW_KEY_1:
			{
				BuildingSystem::shared()->createNewToeHold(getPos() + m_camera->getForward() * 5);
			}
			break;
		case TZW_KEY_2:
			{
				BuildingSystem::shared()->cook();
			}
			break;
		case TZW_KEY_R:
			{
				auto testCylinder = new CylinderPrimitive(0.5, 0.5, 1);
				g_GetCurrScene()->addNode(testCylinder);
				testCylinder->setPos(getPos().x - 3, getPos().y + 10, getPos().z);
				auto rigCylinder = PhysicsMgr::shared()->createRigidBodyCylinder(1.0, 0.5, 0.5, 1.0, testCylinder->getTranslationMatrix());
				rigCylinder->attach(testCylinder);
				break;


				//left front wheel
				auto wheelA = new SpherePrimitive(0.7,10);
				g_GetCurrScene()->addNode(wheelA);
				wheelA->setPos(getPos().x - 3, getPos().y + 10, getPos().z);
				auto rigA = PhysicsMgr::shared()->createRigidBodySphere(1.0, wheelA->getTranslationMatrix(), wheelA->radius());
				rigA->attach(wheelA);
				  
				//right front wheel
				auto wheelB = new SpherePrimitive(0.7,10);
				g_GetCurrScene()->addNode(wheelB);
				wheelB->setPos(getPos().x + 3, getPos().y + 10, getPos().z);
				auto mat = wheelB->getTranslationMatrix();
				auto rigB = PhysicsMgr::shared()->createRigidBodySphere(1.0, mat, wheelB->radius());
				rigB->attach(wheelB);


				//left back wheel
				auto wheelC = new SpherePrimitive(0.7,10);
				g_GetCurrScene()->addNode(wheelC);
				wheelC->setPos(getPos().x - 3, getPos().y + 10, getPos().z);
				auto rigC = PhysicsMgr::shared()->createRigidBodySphere(1.0, wheelC->getTranslationMatrix(), wheelC->radius());
				rigC->attach(wheelC);

				//right back wheel
				auto wheelD = new SpherePrimitive(0.7,10);
				g_GetCurrScene()->addNode(wheelD);
				wheelD->setPos(getPos().x + 3, getPos().y + 10, getPos().z);
				auto rigD = PhysicsMgr::shared()->createRigidBodySphere(1.0, wheelD->getTranslationMatrix(), wheelD->radius());
				rigD->attach(wheelD);


				float carWidth = 3.0f;
				float carDepth = 4.0f;
				auto chasis = new CubePrimitive(carWidth, carDepth, 0.2f);
				g_GetCurrScene()->addNode(chasis);
				chasis->setPos(getPos().x, getPos().y + 10, getPos().z);
				auto mat2 = wheelB->getTranslationMatrix();
				auto rigChasis = PhysicsMgr::shared()->createRigidBody(1.0, mat2, chasis->getAABB());
				rigChasis->attach(chasis);

				//front
				//connected to left wheel
				constraint1 = PhysicsMgr::shared()->createHingeConstraint(rigChasis, rigA, vec3(-carWidth / 2.0f, 0.0, carDepth / 2.0f), vec3(0.7 + 0.05, 0.0, 0.0), vec3(1.0, 0.0, 0.0), vec3(1.0, 0.0, 0.0), false);

				//connected to right wheel
				constraint2 = PhysicsMgr::shared()->createHingeConstraint(rigChasis, rigB, vec3(carWidth / 2.0f, 0.0, carDepth / 2.0f), vec3(-(0.7 + 0.05), 0.0, 0.0), vec3(1.0, 0.0, 0.0), vec3(1.0, 0.0, 0.0), false);

				//back
				//connected to left wheel
				PhysicsMgr::shared()->createHingeConstraint(rigChasis, rigC, vec3(-carWidth / 2.0f, 0.0, -carDepth / 2.0f), vec3(0.7 + 0.05, 0.0, 0.0), vec3(1.0, 0.0, 0.0), vec3(1.0, 0.0, 0.0), false);

				//connected to right wheel
				PhysicsMgr::shared()->createHingeConstraint(rigChasis, rigD, vec3(carWidth / 2.0f, 0.0, -carDepth / 2.0f), vec3(-(0.7 + 0.05), 0.0, 0.0), vec3(1.0, 0.0, 0.0), vec3(1.0, 0.0, 0.0), false);

				wheelFrontLeft = rigA;

				wheelFrontRight = rigB;
			}
			break;
		case TZW_KEY_Q:
			{
				m_enableGravity = !m_enableGravity;
				m_camera->setIsEnableGravity(m_enableGravity);
			}
			break;
		case TZW_KEY_F:
			{
				BuildingSystem::shared()->createPlaceByHit(getPos(),m_camera->getForward(), 15);
				break;
				//std::vector<Drawable3D *> list;
				//auto pos = this->getPos();
				//AABB aabb;
				//aabb.update(vec3(pos.x - 10, pos.y - 10, pos.z - 10));
				//aabb.update(vec3(pos.x + 10, pos.y + 10, pos.z + 10));
				//g_GetCurrScene()->getRange(&list, aabb);
				//if (!list.empty())
				//{
				//	Drawable3DGroup group(&list[0], list.size());
				//	Ray ray(pos, m_camera->getForward());
				//	vec3 hitPoint;
				//	auto chunk = static_cast<Chunk *>(group.hitByRay(ray, hitPoint));
				//	if (chunk)
				//	{
				//		chunk->deformSphere(hitPoint, -0.1, 2.0f);
				//	}
				//}
			}
			break;
		case TZW_KEY_E:
			{
				BuildingSystem::shared()->createBearByHit(getPos(),m_camera->getForward(), 15);
				break;
				//std::vector<Drawable3D *> list;
				//auto pos = this->getPos();
				//AABB aabb;
				//aabb.update(vec3(pos.x - 10, pos.y - 10, pos.z - 10));
				//aabb.update(vec3(pos.x + 10, pos.y + 10, pos.z + 10));
				//g_GetCurrScene()->getRange(&list, aabb);
				//Drawable3DGroup group(&list[0], list.size());
				//Ray ray(pos, m_camera->getForward());
				//vec3 hitPoint;
				//auto chunk = static_cast<Chunk *>(group.hitByRay(ray, hitPoint));
				//if (chunk)
				//{
				//	chunk->deformSphere(hitPoint, 0.1, 2.0f);
				//}
			}
			break;
		default:
			break;
		}
		return false;
	}

	bool CubePlayer::onMousePress(int button, vec2 thePos)
	{
		return true;
	}

	void CubePlayer::modeSwitch(Mode newMode)
	{
		m_currMode = newMode;
	}
} // namespace tzw
