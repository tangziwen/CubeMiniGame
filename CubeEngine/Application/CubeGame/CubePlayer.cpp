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
#include "GameItemMgr.h"
#include "AssistDrawSystem.h"
#include "Shader/ShaderMgr.h"
#include "PartSurfaceMgr.h"
#include "PaintGun.h"
#include "Base/TranslationMgr.h"
#include "3D/Primitive/RightPrismPrimitive.h"
#include "ButtonPart.h"
#include "SwitchPart.h"
#include "3D/Particle/ParticleEmitter.h"
#include "3D/Particle/ParticleInitSizeModule.h"
#include "3D/Particle/ParticleInitVelocityModule.h"
#include "3D/Particle/ParticleInitLifeSpanModule.h"
#include "3D/Particle/ParticleInitAlphaModule.h"
#include "3D/Particle/ParticleUpdateColorModule.h"
#include "3D/Particle/ParticleUpdateSizeModule.h"
#include "3D/Particle/ParticleInitPosModule.h"
#include "BulletMgr.h"

#include "3D/Terrain/MarchingCubes.h"
#include "3D/Terrain/TransVoxel.h"
#include "AudioSystem/AudioSystem.h"
#include "FastNoise/FastNoise.h"
#include "GameUISystem.h"
#include "Lighting/PointLight.h"
#include <Application\CubeGame\GamePartRenderNode.h>

#include "Gun/FPGun.h"
#include "Gun/FPGunMgr.h"
namespace tzw
{

	CubePlayer::CubePlayer(Node* mainRoot):m_seatViewMode(InSeatViewMode::ORBIT)
	{
		GameItemMgr::shared();
		initSlots();
		m_paintGun = new PaintGun();
		m_paintGun->color = vec3(1, 1, 1);
		m_paintGun->m_surface = PartSurfaceMgr::shared()->getItem("foam grip");
		m_currMode = Mode::MODE_DEFORM_SPHERE;
		GUISystem::shared()->addObject(this);
		FPSCamera* camera = FPSCamera::create(g_GetCurrScene()->defaultCamera(), true);
		camera->setCamPos(vec3(5, 20.0, -5));
		mainRoot->addChild(camera);
		g_GetCurrScene()->setDefaultCamera(camera);
		m_camera = camera;
		camera->reCache();
		m_camera->setIsEnableGravity(true);
		m_camera->m_onHitGround = std::bind(&CubePlayer::onHitGround, this);
		m_currSelectItemIndex = 0;
		m_currSelectedItem = nullptr;

		m_orbitcamera = OrbitCamera::create(g_GetCurrScene()->defaultCamera());
		m_fpvCamera = FPSCamera::create(g_GetCurrScene()->defaultCamera(), false);
		mainRoot->addChild(m_orbitcamera);

		auto pos = getPos();
		oldPosX = pos.x / ((MAX_BLOCK + 1) * BLOCK_SIZE);
		oldPosZ = (-1.0f * pos.z) / ((MAX_BLOCK + 1) * BLOCK_SIZE);
		//Sky::shared()->setCamera(m_camera);
		EventMgr::shared()->addFixedPiorityListener(this);

		m_enableGravity = true;
		m_currPointPart = nullptr;

		m_previewItem = new PreviewItem();
		m_gunModel = FPGunMgr::shared()->loadGun("Guns/AR.json");//FPGunMgr::shared()->loadGun("Guns/Hammer.json");//

		m_gunDict["AR"] = m_gunModel;
		m_gunDict["Hammer"] = FPGunMgr::shared()->loadGun("Guns/Hammer.json");
		ADSPos = vec3(0.0,-0.015, -0.07);
		hipPos = vec3(0.12,0.0, -0.22);

		
		//m_gunModel->setPos(0.12,0.6, -0.22);
		//m_gunModel->setRotateE(vec3(0, -90.f, 0));
		float scale = 0.01;
		//m_gunModel->setScale(vec3(scale, scale, scale));
		//m_gunModel->setIsAccpectOcTtree(false);
		m_camera->addChild(m_gunModel->getModel());
		m_camera->addChild(m_gunDict["Hammer"]->getModel());

		m_footstep = AudioSystem::shared()->createSound("Audio/footstep.wav");
		m_hitGroundSound = AudioSystem::shared()->createSound("Audio/land.ogg");


		auto treeModel = Model::create("Models/tree/tree.tzw");
		g_GetCurrScene()->addNode(treeModel);
		treeModel->getMat(0)->setTex("DiffuseMap", TextureMgr::shared()->getByPath("Models/tree/bark.jpg", true));
		treeModel->getMat(1)->setTex("DiffuseMap", TextureMgr::shared()->getByPath("Models/tree/twig.png", true));
		treeModel->setPos(vec3(5, 5, -5));
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
		m_camera->setCamPos(newPos);
	}

	void CubePlayer::onFrameUpdate(float dt)
	{
		AudioSystem::shared()->setListenerParam(getPos(),m_camera->getForward(), vec3(0,1, 0));
		static float theTime = 0.0f;
		static float stepLoopTime = 0.0f;
		//vec3 oldPos = m_gunModel->getPos();
		float offset = 0.002;
		if(isAdsMode)
		{
			offset = 0.0005;
		}
		float freq = 1.2;
		if (m_camera->getIsMoving() && m_camera->isOnGround())
		{
			stepLoopTime += dt;
			if(stepLoopTime > 0.35)
			{
				auto event = m_footstep->playWithOutCare();
				event.setVolume(1.2f);
				stepLoopTime = 0.0;
			}
			if(isAdsMode)
			{
				offset = 0.0008;
			}
			else
			{
				offset = 0.006;
			}
			freq = 8;
		}
		theTime += freq * dt;
		if(isAdsMode)
		{
			//m_gunModel->setPos(vec3(ADSPos.x, ADSPos.y + sinf(theTime) * offset, ADSPos.z));
		}
		else
		{
			//m_gunModel->setPos(vec3(hipPos.x, hipPos.y + sinf(theTime) * offset, hipPos.z));
		}
		m_gunModel->tick(m_camera->getIsMoving() && m_camera->isOnGround(), dt);
		if (checkIsNeedUpdateChunk())
		{
			GameWorld::shared()->loadChunksAroundPlayer();
		}
		auto seat = BuildingSystem::shared()->getCurrentControlPart();
		if(seat && seat->getIsActivate()) 
		{
			auto label = GameUISystem::shared()->getCrossHairTipsInfo();
			if(!label) return;
			label->setIsVisible(false);
			return;
		}
		m_previewItem->handlePreview(m_currSelectedItem, getPos(),m_camera->getTransform().forward());
		GamePart * part = nullptr;
		bool isInXray = BuildingSystem::shared()->isIsInXRayMode();
		if(isInXray)
		{
			part = BuildingSystem::shared()->rayTestPartXRay(getPos(), m_camera->getTransform().forward(), 10.0);
		}else
		{
			part = BuildingSystem::shared()->rayTestPart(getPos(), m_camera->getTransform().forward(), 10.0);
		}
		if(part)
		{
			if(m_currPointPart != part)
			{
				if(m_currPointPart)
				{
					static_cast<GamePartRenderNode *>(m_currPointPart->getNode())->setIsHovering(false);
				}
				

				m_currPointPart = part;
				if(m_currPointPart)
				{
					static_cast<GamePartRenderNode *>(m_currPointPart->getNode())->setIsHovering(true);
				}
				updateCrossHairTipsInfo();
			}
		}else
		{
			if(m_currPointPart)
			{
				static_cast<GamePartRenderNode *>(m_currPointPart->getNode())->setIsHovering(false);
				m_currPointPart = nullptr;
				updateCrossHairTipsInfo();
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

		default:
		  break;
		}
		return false;
	}

	bool CubePlayer::onKeyRelease(int keyCode)
	{
		//if (MainMenu::shared()->isVisible()) return false;
		switch (keyCode)
		{
	    case TZW_KEY_ESCAPE:
	        GameUISystem::shared()->closeCurrentWindow();
	        break;
        case TZW_KEY_H:
			{
        		//m_camera->setUseCustomFrustumUpdate(true);
        		
        		auto pointLight = new PointLight();
        		pointLight->setRadius(10);
        		pointLight->setLightColor(vec3(1, 0, 0));
        		pointLight->setPos(getPos());
        		g_GetCurrScene()->addNode(pointLight);
        		auto mdata = m_camera->getTransform().data();
				auto bullet = BulletMgr::shared()->fire(m_camera->getWorldPos() + vec3(mdata[0], mdata[1], mdata[2]) * 0.25, m_camera->getForward(), 15, BulletType::HitScanLaser);

            }
			break;
        case TZW_KEY_G:
			{
				auto model = new Model();
        		model->initWithFile("wheat_02.tzw");
				model->setPos(getPos());
        		//model->setScale(100, 100, 100);
        		model->setIsAccpectOcTtree(true);
        		g_GetCurrScene()->addNode(model);
            }
			break;
        case TZW_KEY_TAB:
			{
        		if(!GUISystem::shared()->isUiCapturingInput())
				GameUISystem::shared()->openMainMenu();
            }
			break;
        case TZW_KEY_X:
			{
				m_camera->setIsEnableGravity(!m_camera->getIsEnableGravity());
			}
			break;
		case TZW_KEY_T:
			{
				if(!GUISystem::shared()->isUiCapturingInput())
				{
					auto toggleXray = !BuildingSystem::shared()->isIsInXRayMode();
					BuildingSystem::shared()->setIsInXRayMode(toggleXray);
					AssistDrawSystem::shared()->setIsShowAssistInfo(toggleXray);
				}
			}
			break;
		case TZW_KEY_V:
			{
				if(getSeatViewMode() == InSeatViewMode::ORBIT) 
				{
					setSeatViewMode(InSeatViewMode::FPV);
				}
				else
				{
					setSeatViewMode(InSeatViewMode::ORBIT);
				}
				auto seat = BuildingSystem::shared()->getCurrentControlPart();
				if(seat && seat->getIsActivate()) 
				{
					handleSitDown();
                }
			}
			break;
			case TZW_KEY_P:
			{
				// auto cube = new CubePrimitive(20.f, 20.0f, 1.0f);
				// 	cube->setPos(getPos());
				// 	g_GetCurrScene()->addNode(cube);
				GameUISystem::shared()->setWindowShow(WindowType::PAINTER, true);
			}
			break;
		default:
			break;
		}
		return false;
	}

	bool CubePlayer::onMousePress(int button, vec2 thePos)
	{
		printf("button %d\n", button);
		if(button == 1)
		{
			isAdsMode = !isAdsMode;
			m_gunModel->toggleADS(false);
		}
		
		if (GameUISystem::shared()->isVisible())
				return false;
		return true;
	}

	void CubePlayer::modeSwitch(Mode newMode)
	{
		m_currMode = newMode;
	}

	void CubePlayer::initSlots()
	{
		m_itemSlots.push_back(GameItemMgr::shared()->getItem("Block"));
		m_itemSlots.push_back(GameItemMgr::shared()->getItem("Wheel"));
		m_itemSlots.push_back(GameItemMgr::shared()->getItem("Bearing"));
		m_itemSlots.push_back(GameItemMgr::shared()->getItem("TerrainFormer"));
	}

	vec3 CubePlayer::getForward() const
	{
		return m_camera->getForward();
	}

	void CubePlayer::handleSitDown()
	{

		switch(m_seatViewMode)
		{
		case InSeatViewMode::FPV:
			g_GetCurrScene()->setDefaultCamera(m_fpvCamera);
		break;
		case InSeatViewMode::ORBIT:
			g_GetCurrScene()->setDefaultCamera(m_orbitcamera);
		break;
		default: ;
		}
	}
	
	void CubePlayer::sitDownToGamePart(GamePart * part)
	{
		m_camera->removeFromParent();
		m_camera->setEnableFPSFeature(false);
		m_camera->pausePhysics();
		tlog("setRotate fuck fuck%s",m_camera->getRotateE().getStr().c_str());
		//m_camera->setIsEnableGravity(false);
		//part->getNode()->addChild(m_camera);
		//m_camera->setPos(0, 0, 0);
		//m_camera->setRotateE(0, 0, 0);
		//m_camera->reCache();

		//fpv setup
		m_fpvCamera->setIsEnableGravity(false);
		part->getNode()->addChild(m_fpvCamera);
		auto Rotate = part->getNode()->getRotateE();
		m_fpvCamera->setPos(0, 0.5, 0.1);
		m_fpvCamera->setRotateE(0, 180, 0);
		m_fpvCamera->reCache();
		
		//orbit setup
		m_orbitcamera->resetDirection();
		m_orbitcamera->setFocusNode(part->getNode());

		handleSitDown();
	}

	void CubePlayer::standUpFromGamePart(GamePart * part)
	{
		//leave
		m_camera->setIsEnableGravity(true);
		m_camera->setEnableFPSFeature(true);
		m_camera->resumePhysics();
		auto aabb = part->m_parent->getAABBInWorld();
		auto dist = aabb.half().z;
		m_camera->setCamPos(aabb.centre() + vec3(0,0,-1) * (dist + 0.5));
		m_camera->lookAt(part->getWorldPos());
		tlog("fuck fuck fuck%s",m_camera->getRotateE().getStr().c_str());
		GameWorld::shared()->getMainRoot()->addChild(m_camera);
		m_orbitcamera->setFocusNode(nullptr);

		m_fpvCamera->removeFromParent();
		
		g_GetCurrScene()->setDefaultCamera(m_camera);
	}

	void CubePlayer::removePartByAttach(Attachment* attach)
	{
		BuildingSystem::shared()->removePartByAttach(attach);
		m_currPointPart = nullptr;
		updateCrossHairTipsInfo();
	}

	void CubePlayer::removePart(GamePart* part)
	{
		AudioSystem::shared()->playOneShotSound(AudioSystem::DefaultOneShotSound::CLINK_REMOVE);
		BuildingSystem::shared()->removePart(part);
		m_currPointPart = nullptr;
		updateCrossHairTipsInfo();
	}

	void CubePlayer::removeAllBlocks()
	{
		BuildingSystem::shared()->removeAll();
		m_currPointPart = nullptr;
	}

	void CubePlayer::updateCrossHairTipsInfo()
	{
		auto label = GameUISystem::shared()->getCrossHairTipsInfo();
		if(!label) return;
		auto item = m_currSelectedItem;
		if(!item)
		{
			label->setIsVisible(false);
			return;
		}
		bool isNeedSpecialShowBySelected = false;
		if(item->isSpecialFunctionItem())
		{
			isNeedSpecialShowBySelected = true;
			switch(item->m_type)
			{
			case GamePartType::GAME_PART_LIFT:
				{
					auto lift = BuildingSystem::shared()->getLift();
					//如果当前指向的方块有属性面板，也不显示
					if(m_currPointPart && m_currPointPart !=lift && m_currPointPart->getItem() && m_currPointPart->getItem()->hasAttributePanel())
					{
						isNeedSpecialShowBySelected = false;
					}
					else
					{
						if(!lift)
						{
							isNeedSpecialShowBySelected = true;
							if(BuildingSystem::shared()->getStoreIslandGroup()) 
							{
								label->setString(TR(u8"放置收纳对象"));
							}
							else
							{
								if(m_currPointPart)
								{
									label->setString(TR(u8"收纳"));
								}
								else
								{
									label->setString(TR(u8"对准空地放置，对准载具收纳放置"));
								}
							}
						}
						else
						{
							if(m_currPointPart && m_currPointPart != lift && !lift->m_effectedIslandGroup)//如果升降机上没有放置载具
							{
								isNeedSpecialShowBySelected = true;
								label->setString(TR(u8"放置到升降机"));
							}
							else if(m_currPointPart && m_currPointPart != lift && m_currPointPart->getVehicle()  == lift->m_effectedIslandGroup)//如果升降机上载具等于自己
							{
								isNeedSpecialShowBySelected = true;
								label->setString(TR(u8"放下到地面"));
							}
							else
							{
								isNeedSpecialShowBySelected = false;
							}
						}
					}

				}
			break;
			case GamePartType::SPECIAL_PART_PAINTER:
				{
					label->setString(TR(u8"(左键) 喷漆 \n(右键) 喷涂面板"));
					isNeedSpecialShowBySelected = true;
				}
			break;
			case GamePartType::SPECIAL_PART_DIGGER:
				{
					label->setString(TR(u8"(左键) 挖掘 \n(右键) 填充"));
					isNeedSpecialShowBySelected = true;
				}
			break;
			default: ;
			}
		}else
		{
			isNeedSpecialShowBySelected = false;
		}

		if(!isNeedSpecialShowBySelected)
		{
			if(!m_currPointPart) 
			{
				label->setIsVisible(false);
				return;
	        }
			label->setIsVisible(true);
			switch(m_currPointPart->getType())
			{
			case GamePartType::GAME_PART_LIFT: label->setString(TR(u8"(E) 载具浏览器")); break;
			case GamePartType::GAME_PART_CONTROL: label->setString(TR(u8"(E) 驾驶\n(F) 节点编辑器"));break;
			case GamePartType::GAME_PART_THRUSTER: label->setString(TR(u8"(E) 属性面板"));break;
			case GamePartType::GAME_PART_CANNON: label->setString(TR(u8"(E) 属性面板"));break;
			case GamePartType::GAME_PART_BEARING: label->setString(TR(u8"(E) 属性面板\n(F) 调整方向"));break;
			case GamePartType::GAME_PART_SPRING: label->setString(TR(u8"(E) 属性面板"));break;
			default:
				label->setIsVisible(false);
				break;
			}
		}else
		{
			label->setIsVisible(true);
		}
	}

	void CubePlayer::openCurrentPartInspectMenu()
	{
		if(m_currPointPart && m_currPointPart->isNeedDrawInspect())
		{
			GameUISystem::shared()->openInspectWindow(m_currPointPart);
		}
	}

	GamePart* CubePlayer::getCurrPointPart()
	{
		return m_currPointPart;
	}

	PaintGun* CubePlayer::getPaintGun()
	{
		return m_paintGun;
	}

	void CubePlayer::paint()
	{
		auto part = BuildingSystem::shared()->rayTestPart(getPos(), m_camera->getTransform().forward(), 10.0);
		AudioSystem::shared()->playOneShotSound(AudioSystem::DefaultOneShotSound::SPRAY);
		if(part)
		{
			m_paintGun->paint(part);
		}
		else
		{
			BuildingSystem::shared()->terrainPaint(getPos(), m_camera->getTransform().forward(), 10, m_paintGun->m_matIndex, 1.5);
		}
	}

	void CubePlayer::setCurrSelected(std::string itemName)
	{
		
		tlog("the Item Name is %s",itemName.c_str());
		if (itemName.empty()) 
		{
			m_currSelectedItem = nullptr;
		}
		m_currSelectedItem = GameItemMgr::shared()->getItem(itemName);
		if(m_currSelectedItem && m_currSelectedItem->getTypeInInt() == (int)GamePartType::SPECIAL_PART_FPGUN)
		{
			m_gunModel->getModel()->setIsVisible(false);
			m_gunModel = m_gunDict["AR"];
			m_gunModel->getModel()->setIsVisible(true);
		}
		else
		{
			m_gunModel->getModel()->setIsVisible(false);
			m_gunModel = m_gunDict["Hammer"];
			m_gunModel->getModel()->setIsVisible(true);
		}
		updateCrossHairTipsInfo();
		if(!m_currSelectedItem) return;
		if(itemName == "Painter") return;
		m_previewItem->setPreviewItem(GameItemMgr::shared()->getItem(itemName));
	}

	void CubePlayer::setPreviewAngle(float angle)
	{
		m_previewItem->setPreviewAngle(angle);
		tlog("%f", angle);
	}

	float CubePlayer::getPreviewAngle() const
	{
		return m_previewItem->getPreviewAngle();
	}

	PreviewItem* CubePlayer::getPreviewItem()
	{
		return m_previewItem;
	}

	void CubePlayer::pressButton(GamePart* buttonPart)
	{
		tlog("press Button!", buttonPart);
		static_cast<ButtonPart *>(buttonPart)->onPressed();
		
	}

	void CubePlayer::releaseButton(GamePart* buttonPart)
	{
		tlog("release Button!", buttonPart);
		static_cast<ButtonPart *>(buttonPart)->onReleased();
	}

	void CubePlayer::releaseSwitch(GamePart* switchPart)
	{
		tlog("release Button!", switchPart);
		static_cast<SwitchPart *>(switchPart)->onToggle();
	}

	CubePlayer::InSeatViewMode CubePlayer::getSeatViewMode() const
	{
		return m_seatViewMode;
	}

	void CubePlayer::setSeatViewMode(const InSeatViewMode seatViewMode)
	{
		m_seatViewMode = seatViewMode;
	}

	void CubePlayer::setIsOpenJetPack(bool isOpen)
	{
		m_camera->setIsEnableGravity(!isOpen);
	}



	bool CubePlayer::onScroll(vec2 offset)
	{
		m_orbitcamera->zoom(-1.f * offset.getY() * 0.25);
		return true;
	}

	GameItem* CubePlayer::getCurSelectedItem()
	{
		return m_currSelectedItem;
	}

	Quaternion CubePlayer::getRotateQ() const
	{
		return m_camera->getRotateQ();
	}

	void CubePlayer::setRotateQ(const Quaternion& rotateQ)
	{
		m_camera->setRotateQ(rotateQ);
	}

	void CubePlayer::setRotateQ(const vec4& rotateQInV4)
	{
		m_camera->setRotateQ(rotateQInV4);
	}

	void CubePlayer::onHitGround()
	{
		tlog("onHigground");
		m_hitGroundSound->playWithOutCare();
	}
} // namespace tzw
