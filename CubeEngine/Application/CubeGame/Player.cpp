#include "Player.h"
#include "EngineSrc/Scene/SceneMgr.h"
#include "EngineSrc/3D/Model/Model.h"
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
    m_gunModel = Model::create("./Res/TestRes/shotgun.tzw");
    m_gunModel->setScale(vec3(0.01, 0.01,0.01));
    m_gunModel->setRotateE(vec3(0, 90, 0));
    m_gunModel->setPos(vec3(0.13,-0.18,-0.12));
    m_camera->addChild(m_gunModel);
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
}
} // namespace tzw
