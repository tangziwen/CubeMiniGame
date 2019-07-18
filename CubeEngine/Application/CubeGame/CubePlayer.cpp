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
#include "ItemMgr.h"
namespace tzw
{

	CubePlayer::CubePlayer(Node* mainRoot)
	{
		ItemMgr::shared();
		initSlots();
		m_currMode = Mode::MODE_DEFORM_SPHERE;
		GUISystem::shared()->addObject(this);
		FPSCamera* camera = FPSCamera::create(g_GetCurrScene()->defaultCamera());
		camera->setPos(vec3(5, 50, -5));
		mainRoot->addChild(camera);
		g_GetCurrScene()->setDefaultCamera(camera);
		m_camera = camera;
		camera->reCache();
		m_camera->setIsEnableGravity(true);
		m_currSelectItemIndex = 0;

		const auto numMeshes = 5;
        for (auto i = 0; i < numMeshes ; i++) {
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
		return false;
	}

	bool CubePlayer::onKeyRelease(int keyCode)
	{
		if (MainMenu::shared()->isVisible()) return false;
		switch (keyCode)
		{
		case TZW_KEY_R:
			{
				BuildingSystem::shared()->flipBearingByHit(getPos(), m_camera->getForward(), 15);
				break;
			}
			break;
		case TZW_KEY_Q:
			{
				m_enableGravity = !m_enableGravity;
				m_camera->setIsEnableGravity(m_enableGravity);
			}
			break;
		case TZW_KEY_H:
			{
				BuildingSystem::shared()->cook();
			}
			break;
        case TZW_KEY_L:
            {
                m_seatAngle += 90.0;
				if(m_seatAngle >= 360.0) m_seatAngle = 0.0;
            }
		break;
		default:
			break;
		}
		return false;
	}

	bool CubePlayer::onMousePress(int button, vec2 thePos)
	{
		if (MainMenu::shared()->isVisible())
				return false;
		//switch(button) 
		//{
		//case 0://left mouse
		//	handleItemPrimaryUse(m_itemSlots[m_currSelectItemIndex]);
		//	break;
		//case 1://right mouse
		//	handleItemSecondaryUse(m_itemSlots[m_currSelectItemIndex]);
		//	break;
		//default: break;
		//}
		return true;
	}

	void CubePlayer::modeSwitch(Mode newMode)
	{
		m_currMode = newMode;
	}

	void CubePlayer::initSlots()
	{
		m_itemSlots.push_back(ItemMgr::shared()->getItem("Block"));
		m_itemSlots.push_back(ItemMgr::shared()->getItem("Wheel"));
		m_itemSlots.push_back(ItemMgr::shared()->getItem("Bearing"));
		m_itemSlots.push_back(ItemMgr::shared()->getItem("TerrainFormer"));
	}
	void CubePlayer::handleItemPrimaryUse(GameItem * item)
	{
		if (item->m_class == "PlaceableBlock")
		{
			BuildingSystem::shared()->placeItem(item, getPos(), m_camera->getForward(), 15);
		}
		else if (item->m_class == "TerrainForm")
		{
			std::vector<Drawable3D *> list;
			auto pos = this->getPos();
			AABB aabb;
			aabb.update(vec3(pos.x - 10, pos.y - 10, pos.z - 10));
			aabb.update(vec3(pos.x + 10, pos.y + 10, pos.z + 10));
			g_GetCurrScene()->getRange(&list, aabb);
			if (!list.empty())
			{
				Drawable3DGroup group(&list[0], list.size());
				Ray ray(pos, m_camera->getForward());
				vec3 hitPoint;
				auto chunk = static_cast<Chunk *>(group.hitByRay(ray, hitPoint));
				if (chunk)
				{
					chunk->deformSphere(hitPoint, -0.5, 3.0f);
				}
			}
		}
	}

	void CubePlayer::handleItemSecondaryUse(GameItem * item)
	{
		if (item->m_class == "PlaceableBlock")
		{
			//BuildingSystem::shared()->removePartByHit(getPos(), m_camera->getForward(), 15);
		}
		else if (item->m_class == "TerrainForm")
		{
			std::vector<Drawable3D *> list;
			auto pos = this->getPos();
			AABB aabb;
			aabb.update(vec3(pos.x - 10, pos.y - 10, pos.z - 10));
			aabb.update(vec3(pos.x + 10, pos.y + 10, pos.z + 10));
			g_GetCurrScene()->getRange(&list, aabb);
			if (!list.empty())
			{
				Drawable3DGroup group(&list[0], list.size());
				Ray ray(pos, m_camera->getForward());
				vec3 hitPoint;
				auto chunk = static_cast<Chunk *>(group.hitByRay(ray, hitPoint));
				if (chunk)
				{
					chunk->deformSphere(hitPoint, 0.5, 3.0f);
				}
			}
		}
	}

	vec3 CubePlayer::getForward() const
	{
		return m_camera->getForward();
	}

	void CubePlayer::attachCamToGamePart(GamePart * part)
	{
		m_camera->removeFromParent();
		m_camera->setIsEnableGravity(false);
		part->getNode()->addChild(m_camera);
		m_camera->setPos(0, 0, 0);
		m_camera->setRotateE(0, 0, 0);
		m_camera->reCache();
	}

	void CubePlayer::attachCamToWorld()
	{
		m_camera->removeFromParent();
		GameWorld::shared()->getMainRoot()->addChild(m_camera);
	}
} // namespace tzw
