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

	m_placeHolder = new CubePrimitive(1.0,1.0,1.0);
	m_placeHolder->setIsHitable(false);
	g_GetCurrScene()->addNode(m_placeHolder);
	m_placeHolder->setPos(0, 0, -5);
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
	case GLFW_KEY_F:
		{

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
				chunk->deformSphere(hitPoint, 100.0, 2.0f);
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


	auto cube = new CubePrimitive(1.0, 1.0, 1.0);
	g_GetCurrScene()->addNode(cube);
	cube->setPos(m_placeHolder->getPos());
	return true;
	std::vector<Drawable3D *> list;
	auto pos = this->getPos();
	AABB aabb;
	aabb.update(vec3(pos.x -10,pos.y- 10,pos.z - 10));
	aabb.update(vec3(pos.x +10,pos.y + 10 ,pos.z + 10));
	g_GetCurrScene()->getRange(&list,aabb);
	Drawable3DGroup group(&list[0],list.size());
	Ray ray(pos,m_camera->getForward());
	vec3 hitPoint;
	group.hitByRay(ray,hitPoint);
	m_placeHolder->setPos(hitPoint);
	//auto chunk = dynamic_cast<Chunk *>(group.hitByRay(ray,hitPoint));
	//if(chunk)
	//{
	//	auto before = clock();
	//	chunk->deformSphere(hitPoint, -100.0, 2.0f);
	//	auto delta = clock() - before;
	//}

	//} 
	//audio->Play();
	//auto bullet = new Projectile(Projectile::ProjectileType::SimpleBullet);
	//bullet->launch(getPos(), m_camera->getForward(), 55);

	//auto sphere = new SpherePrimitive(3.0,30);
	//g_GetCurrScene()->addNode(sphere);
	//sphere->setPos(m_placeHolder->getWorldPos(vec3(0, 0, 0)));
	return true;
}

void CubePlayer::modeSwitch(Mode newMode)
{
	m_currMode = newMode;
}

void CubePlayer::updatePlaceHolder()
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
		if (result->getTypeID() == TYPE_CHUNK)
		{
			m_placeHolder->setPos(hitPoint + vec3(0, 0.5, 0.0));
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
				m_placeHolder->setPos(aabb.centre() + vec3(0, 1.0, 0));
				break;
			case tzw::RayAABBSide::down:
				m_placeHolder->setPos(aabb.centre() + vec3(0, -1.0, 0));
				break;
			case tzw::RayAABBSide::left:
				m_placeHolder->setPos(aabb.centre() + vec3(-1.0, 0, 0));
				break;
			case tzw::RayAABBSide::right:
				m_placeHolder->setPos(aabb.centre() + vec3(1.0, 0, 0));
				break;
			case tzw::RayAABBSide::front:
				m_placeHolder->setPos(aabb.centre() + vec3(0, 0, 1.0));
				break;
			case tzw::RayAABBSide::back:
				m_placeHolder->setPos(aabb.centre() + vec3(0, 0, -1.0));
				break;
			default:
				break;
			}
		}
	}
	else
	{

		m_placeHolder->setPos(m_camera->getForward() * 5.0 + m_camera->getPos());
	}
	
}

} // namespace tzw
