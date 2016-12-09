#include "CubePlayer.h"
#include "EngineSrc/Scene/SceneMgr.h"
#include "EngineSrc/3D/Model/Model.h"
#include "GameConfig.h"
#include "GameWorld.h"
#include "EngineSrc/Event/EventMgr.h"
#include <iostream>
#include "3D/Primitive/CubePrimitive.h"
#include "Action/MoveBy.h"
namespace tzw {

CubePlayer::CubePlayer(Node *mainRoot)
{
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
    m_gunModel->setPos(vec3(0.08,-0.2,-0.25));
    m_camera->addChild(m_gunModel);

	auto pos = getPos();
	oldPosX = pos.x / ((MAX_BLOCK + 1) * BLOCK_SIZE);
	oldPosZ = (-1.0f * pos.z) / ((MAX_BLOCK + 1) * BLOCK_SIZE);

	EventMgr::shared()->addFixedPiorityListener(this);
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
	return false;
}

bool CubePlayer::onKeyRelease(int keyCode)
{
	switch(keyCode)
	{
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
				auto before = clock();
				chunk->deformAround(hitPoint, -1);
				auto delta = clock() - before;
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
				chunk->deformAround(hitPoint, 1);
			}
		}
		break;
	default:
		break;
	}
	return false;
}

bool CubePlayer::onMousePress(int button,vec2 pos)
{
	CubePrimitive * cube = new CubePrimitive(0.2, 0.2, 0.2);
	g_GetCurrScene()->addNode(cube);
	cube->setPos(getPos());
	cube->runAction(new MoveBy(3.0, m_camera->getForward() * 55));
	return true;
}

} // namespace tzw
