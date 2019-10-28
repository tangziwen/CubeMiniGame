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

#include "EngineSrc/Collision/PhysicsMgr.h"
#include "BuildingSystem.h"
#include "ItemMgr.h"
#include "AssistDrawSystem.h"

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


		m_orbitcamera = OrbitCamera::create(g_GetCurrScene()->defaultCamera());
		mainRoot->addChild(m_orbitcamera);

		auto pos = getPos();
		oldPosX = pos.x / ((MAX_BLOCK + 1) * BLOCK_SIZE);
		oldPosZ = (-1.0f * pos.z) / ((MAX_BLOCK + 1) * BLOCK_SIZE);
		Sky::shared()->setCamera(m_camera);
		EventMgr::shared()->addFixedPiorityListener(this);

		m_enableGravity = true;
		m_currPointPart = nullptr;
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

	void CubePlayer::setPos(vec3 newPos)
	{
		m_camera->setPos(newPos);
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
		auto part = BuildingSystem::shared()->rayTestPart(getPos(), m_camera->getTransform().forward(), 10.0);
		if(part)
		{
			if(m_currPointPart != part)
			{
				part->highLight();
				if(m_currPointPart)
				{
					m_currPointPart->unhighLight();
				}
				m_currPointPart = part;
			}
		}else
		{
			if(m_currPointPart)
			{
				m_currPointPart->unhighLight();
				m_currPointPart = nullptr;
			}
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
			case TZW_KEY_T:
			{
				AssistDrawSystem::shared()->setIsShowAssistInfo(true);
			}
			break;
		default:
		  break;
		}
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
			}
			break;
		case TZW_KEY_Q:
			{
				m_enableGravity = !m_enableGravity;
				m_camera->setIsEnableGravity(m_enableGravity);
			}
			break;
		case TZW_KEY_T:
			{
				AssistDrawSystem::shared()->setIsShowAssistInfo(false);
			}
			break;
		case TZW_KEY_J:
			{
				BuildingSystem::shared()->replaceToLiftByRay(getPos(), m_camera->getForward(), 15);
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
		m_camera->setEnableFPSFeature(false);
		//m_camera->setIsEnableGravity(false);
		//part->getNode()->addChild(m_camera);
		//m_camera->setPos(0, 0, 0);
		//m_camera->setRotateE(0, 0, 0);
		//m_camera->reCache();

		m_orbitcamera->resetDirection();
		m_orbitcamera->setFocusNode(part->getNode());
		g_GetCurrScene()->setDefaultCamera(m_orbitcamera);
	}

	void CubePlayer::attachCamToWorld()
	{
		m_camera->removeFromParent();
		m_camera->setIsEnableGravity(true);
		m_camera->setEnableFPSFeature(true);
		GameWorld::shared()->getMainRoot()->addChild(m_camera);
		g_GetCurrScene()->setDefaultCamera(m_camera);
	}

	void CubePlayer::removePartByAttach(Attachment* attach)
	{
		BuildingSystem::shared()->removePartByAttach(attach);
		m_currPointPart = nullptr;
	}

	void CubePlayer::removeAllBlocks()
	{
		BuildingSystem::shared()->removeAll();
		m_currPointPart = nullptr;
	}
} // namespace tzw
