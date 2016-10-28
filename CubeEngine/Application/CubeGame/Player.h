#ifndef TZW_PLAYER_H
#define TZW_PLAYER_H

#include "FPSCamera.h"
#include "EngineSrc/3D/Model/Model.h"
namespace tzw {

class Player : public Node
{
public:
    Player(Node * mainRoot);
    FPSCamera *camera() const;
    void setCamera(FPSCamera *camera);
    vec3 getPos();
    virtual void logicUpdate(float dt);
private:
    FPSCamera * m_camera;
    Model * m_gunModel;
};

} // namespace tzw

#endif // TZW_PLAYER_H
