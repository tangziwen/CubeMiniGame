#include "CubePlayer.h"
#include "EngineSrc/Scene/SceneMgr.h"
#include "EngineSrc/3D/Model/Model.h"
#include "GameConfig.h"
#include "GameWorld.h"
#include "EngineSrc/Event/EventMgr.h"
#include <iostream>
#include "3D/Primitive/CubePrimitive.h"
#include "3D/Sky.h"
#include "Projectile.h"
#include "AudioSystem/AudioSystem.h"
#include "Action/RepeatForever.h"
#include "Action/TintTo.h"
namespace tzw {

static Audio * audio;
CubePlayer::CubePlayer(Node *mainRoot)
{
	m_currMode = Mode::MODE_DEFORM_SPHERE;
	audio = new Audio("res/m3-1.wav");
	
    FPSCamera * camera = FPSCamera::create(g_GetCurrScene()->defaultCamera());
    camera->setPos(vec3(5,50,-5));
    mainRoot->addChild(camera);
    g_GetCurrScene()->setDefaultCamera(camera);
    m_camera = camera;
    m_camera->setIsEnableGravity(true);

    //the gun
    m_gunModel = Model::create("./Res/diediedie.tzw");
    m_gunModel->setScale(vec3(0.005, 0.005, 0.005));
    m_gunModel->setRotateE(vec3(0, -90, 0));
    m_gunModel->setPos(vec3(0.04,-0.2,-0.20));
    m_camera->addChild(m_gunModel);

	auto pos = getPos();
	oldPosX = pos.x / ((MAX_BLOCK + 1) * BLOCK_SIZE);
	oldPosZ = (-1.0f * pos.z) / ((MAX_BLOCK + 1) * BLOCK_SIZE);
	Sky::shared()->setCamera(m_camera);
	EventMgr::shared()->addFixedPiorityListener(this);

	initPlaceHolder();
	m_enableGravity = true;
}

FPSCamera *CubePlayer::camera() const
{
    return m_camera;
}

void CubePlayer::setCamera(FPSCamera *camera)
{
    m_camera = camera;
}

vec3 CubePlayer::getPos()
{
    return m_camera->getPos();
}

void CubePlayer::logicUpdate(float dt)
{
    static float  theTime = 0.0f;
    vec3 oldPos = m_gunModel->getPos();
    float offset = 0.002;
    float freq = 1;
    if(m_camera->getIsMoving())
    {
        offset = 0.006;
        freq = 6;
    }
    theTime += freq * dt;
    m_gunModel->setPos(vec3(oldPos.x, -0.2 + sinf(theTime) * offset, oldPos.z));
	if (checkIsNeedUpdateChunk())
	{
		GameWorld::shared()->loadChunksAroundPlayer();
	}
	updatePlaceHolder();
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

bool CubePlayer::onKeyPress(int keyCode)
{

	switch(keyCode)
	{
	case GLFW_KEY_1:
		break;
	case GLFW_KEY_2:
		break;
	case GLFW_KEY_3:
		break;
	case GLFW_KEY_4:
		break;

	}
	return false;
}

bool CubePlayer::onKeyRelease(int keyCode)
{
	switch(keyCode)
	{
	
	case GLFW_KEY_Q:
		{
			m_enableGravity = !m_enableGravity;
			m_camera->setIsEnableGravity(m_enableGravity);
		}
		break;
	case GLFW_KEY_F:
		{
			std::vector<Drawable3D *> list;
			auto pos = this->getPos();
			AABB aabb;
			aabb.update(vec3(pos.x -10,pos.y- 10,pos.z - 10));
			aabb.update(vec3(pos.x +10,pos.y + 10 ,pos.z + 10));
			g_GetCurrScene()->getRange(&list,aabb);
			Drawable3DGroup group(&list[0],list.size());
			Ray ray(pos,m_camera->getForward());
			vec3 hitPoint;
			auto chunk = static_cast<Chunk *>(group.hitByRay(ray,hitPoint));
			if(chunk)
			{
				chunk->deformSphere(hitPoint, -0.1, 2.0f);
			}
		}
		break;
	case GLFW_KEY_E:
		{
			std::vector<Drawable3D *> list;
			auto pos = this->getPos();
			AABB aabb;
			aabb.update(vec3(pos.x -10,pos.y- 10,pos.z - 10));
			aabb.update(vec3(pos.x +10,pos.y + 10 ,pos.z + 10));
			g_GetCurrScene()->getRange(&list,aabb);
			Drawable3DGroup group(&list[0],list.size());
			Ray ray(pos,m_camera->getForward());
			vec3 hitPoint;
			auto chunk = static_cast<Chunk *>(group.hitByRay(ray,hitPoint));
			if(chunk)
			{
				chunk->deformSphere(hitPoint, 0.1, 2.0f);
			}
		}
		break;
	default:
		break;
	}
	return false;
}

bool CubePlayer::onMousePress(int button,vec2 thePos)
{
	switch (button)
	{
	case 0://×ó¼ü
		{
			insertBox();
		}
		break;
	case 1://ÓÒ¼ü
		{
			removeBox();

		}
		break;
	default:
		break;
	}

	return true;
}

void CubePlayer::modeSwitch(Mode newMode)
{
	m_currMode = newMode;
}

void CubePlayer::insertBox()
{
	if(m_placeHolder->getIsVisible())
	{
		auto cube = new CubePrimitive(BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE);
		g_GetCurrScene()->addNode(cube);
		cube->setPos(m_placeHolder->getPos());
	}
}

void CubePlayer::removeBox()
{
	std::vector<Drawable3D *> list;
	auto pos = this->getPos();
	AABB aabb;
	aabb.update(vec3(pos.x - 8,pos.y - 8,pos.z - 8));
	aabb.update(vec3(pos.x + 8,pos.y + 8 ,pos.z + 8));
	g_GetCurrScene()->getRange(&list,aabb);
	Drawable3DGroup group(&list[0],list.size());
	Ray ray(pos,m_camera->getForward());
	vec3 hitPoint;
	auto result = group.hitByRay(ray,hitPoint);
	if (result)
	{
		if (result->getTypeID() != TYPE_CHUNK)
		{
			result->removeFromParent();
		}
	}
}

void CubePlayer::updatePlaceHolder()
{
	std::vector<Drawable3D *> list;
	auto pos = this->getPos();
	AABB aabb;
	aabb.update(vec3(pos.x - 8,pos.y - 8,pos.z - 8));
	aabb.update(vec3(pos.x + 8,pos.y + 8 ,pos.z + 8));
	g_GetCurrScene()->getRange(&list,aabb);
	if (list.empty())
		return;
	Drawable3DGroup group(&list[0],list.size());
	Ray ray(pos,m_camera->getForward());
	vec3 hitPoint;
	auto result = group.hitByRay(ray,hitPoint);
	if (result)
	{
		m_placeHolder->setIsVisible(true);
		if (result->getTypeID() == TYPE_CHUNK)
		{
			if(hitPoint.distance(getPos()) < 8)
			{
				int x = hitPoint.x / BLOCK_SIZE;
				int z = hitPoint.z / BLOCK_SIZE;
				hitPoint.x = BLOCK_SIZE * x;
				hitPoint.z = BLOCK_SIZE * z;
				m_placeHolder->setPos(hitPoint);
			}
			else
			{
				m_placeHolder->setIsVisible(false);
			}
		}
		else
		{
			CubePrimitive * primitive = static_cast<CubePrimitive *>(result);
			auto aabb = result->getAABB();
			RayAABBSide side;
			vec3 hitPoint;
			ray.intersectAABB(aabb,&side, hitPoint);
			switch (side)
			{
			case tzw::RayAABBSide::up:
				m_placeHolder->setPos(aabb.centre() + vec3(0, BLOCK_SIZE, 0));
				break;
			case tzw::RayAABBSide::down:
				m_placeHolder->setPos(aabb.centre() + vec3(0, -BLOCK_SIZE, 0));
				break;
			case tzw::RayAABBSide::left:
				m_placeHolder->setPos(aabb.centre() + vec3(-BLOCK_SIZE, 0, 0));
				break;
			case tzw::RayAABBSide::right:
				m_placeHolder->setPos(aabb.centre() + vec3(BLOCK_SIZE, 0, 0));
				break;
			case tzw::RayAABBSide::front:
				m_placeHolder->setPos(aabb.centre() + vec3(0, 0, BLOCK_SIZE));
				break;
			case tzw::RayAABBSide::back:
				m_placeHolder->setPos(aabb.centre() + vec3(0, 0, -BLOCK_SIZE));
				break;
			default:
				break;
			}
		}
	}
	else
	{
		m_placeHolder->setIsVisible(false);
	}
}

void CubePlayer::initPlaceHolder()
{
	m_placeHolder = new Block();
	m_placeHolder->setIsHitable(false);
	m_placeHolder->setColor(vec4::fromRGB(255, 0, 0, 128));
	m_placeHolder->runAction(new RepeatForever(new TintTo(1.0, vec4::fromRGB(200, 200, 200, 128), vec4::fromRGB(255, 200, 200, 128))));
	m_placeHolder->setIsNeedTransparent(true);
	g_GetCurrScene()->addNode(m_placeHolder);
	m_placeHolder->setPos(0, 0, -5);
}

} // namespace tzw
