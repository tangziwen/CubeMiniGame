#ifndef TZW_PLAYER_H
#define TZW_PLAYER_H

#include "Game/FPSCamera.h"
#include "EngineSrc/3D/Model/Model.h"
namespace tzw {

class CubePlayer : public Node, public EventListener
{
public:
    CubePlayer(Node * mainRoot);
    FPSCamera *camera() const;
    void setCamera(FPSCamera *camera);
    vec3 getPos();
    virtual void logicUpdate(float dt);
	bool checkIsNeedUpdateChunk();
	virtual bool onKeyPress(int keyCode);
	virtual bool onKeyRelease(int keyCode);
	virtual bool onMousePress(int button,vec2 pos);
private:
    FPSCamera * m_camera;
    Model * m_gunModel;
	int oldPosX;
	int oldPosZ;
};

} // namespace tzw

#endif // TZW_PLAYER_H
