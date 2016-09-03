#include "Player.h"
#include "EngineSrc/Scene/SceneMgr.h"
namespace tzw {

Player::Player(Node *mainRoot)
{
    FPSCamera * camera = FPSCamera::create(SceneMgr::shared()->currentScene()->defaultCamera());
    camera->setPos(vec3(0,20,0));
    mainRoot->addChild(camera);
    SceneMgr::shared()->currentScene()->setDefaultCamera(camera);
    m_camera = camera;
    m_camera->setIsEnableGravity(false);
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
} // namespace tzw
