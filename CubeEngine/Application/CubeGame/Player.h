#ifndef TZW_PLAYER_H
#define TZW_PLAYER_H

#include "FPSCamera.h"
namespace tzw {

class Player
{
public:
    Player(Node * mainRoot);
    FPSCamera *camera() const;
    void setCamera(FPSCamera *camera);
    vec3 getPos();
private:
    FPSCamera * m_camera;
};

} // namespace tzw

#endif // TZW_PLAYER_H
