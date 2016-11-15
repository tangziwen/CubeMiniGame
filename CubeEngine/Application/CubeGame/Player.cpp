#include "Player.h"
#include "EngineSrc/Scene/SceneMgr.h"
#include "EngineSrc/3D/Model/Model.h"
#include "GameConfig.h"
#include "GameWorld.h"
namespace tzw {

Player::Player(Node *mainRoot)
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
    m_gunModel->setPos(vec3(0.08,0.15,-0.25));
    m_camera->addChild(m_gunModel);

	auto pos = getPos();
	oldPosX = pos.x / ((MAX_BLOCK + 1) * BLOCK_SIZE);
	oldPosZ = (-1.0f * pos.z) / ((MAX_BLOCK + 1) * BLOCK_SIZE);
}

FPSCamera *Player::camera() const
{
    return m_camera;
}

void Player::setCamera(FPSCamera *camera)
{
    m_camera = camera;
}

vec3 Player::getPos()
{
    return m_camera->getPos();
}

void Player::logicUpdate(float dt)
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
    m_gunModel->setPos(vec3(oldPos.x, -0.20 + sinf(theTime) * offset, oldPos.z));
	if (checkIsNeedUpdateChunk())
	{
		GameWorld::shared()->loadChunksAroundPlayer();
	}
}
bool Player::checkIsNeedUpdateChunk()
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
} // namespace tzw
