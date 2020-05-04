#ifndef TZW_FPSCAMERA_H
#define TZW_FPSCAMERA_H

#include "EngineSrc/Base/Camera.h"
#include "EngineSrc/Event/Event.h"
#include <functional>
#include "EngineSrc/Interface/Drawable3D.h"
#include "EngineSrc/Collision/ColliderEllipsoid.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#include "BulletDynamics/Character/btKinematicCharacterController.h"
#include "Collision/PhysicsListener.h"

namespace tzw {

class FPSCamera : public Camera,public EventListener, public PhysicsListener
{
public:
    FPSCamera(bool isOpenPhysics);
    static FPSCamera * create(Camera * cloneObj, bool isOpenPhysics);
	bool onKeyPress(int keyCode) override;
	bool onKeyRelease(int keyCode) override;
	bool onMouseRelease(int button,vec2 pos) override;
	bool onMousePress(int button,vec2 pos) override;
	bool onMouseMove(vec2 pos) override;
	void logicUpdate(float dt) override;
    vec3 speed() const;
    void setSpeed(const vec3 &speed);

	void recievePhysicsInfo(vec3 pos, Quaternion rot) override;
    vec3 rotateSpeed() const;
    void setRotateSpeed(const vec3 &rotateSpeed);

    float getDistToGround() const;
    void setDistToGround(float value);

    float getGravity() const;
    void setGravity(float gravity);
	void setCamPos(const vec3 &pos);
    bool getIsEnableGravity() const;
    void setIsEnableGravity(bool isEnableGravity);

    float getDistToside() const;
    void setDistToside(float distToside);

    float getMaxFallSpeed() const;
    void setMaxFallSpeed(float maxFallSpeed);
    std::function<void (vec3,vec3,vec3&)> collideCheck;
    bool getEnableFPSFeature() const;
    void setEnableFPSFeature(bool enableFPSFeature);
	void setRotateQ(const Quaternion &rotateQ) override;
    void lookAt( vec3 pos);

    bool getIsMoving() const;
    void setIsMoving(bool isMoving);
	bool isOnGround() const;

	vec3 getTotalSpeed();
	void pausePhysics();
	void resumePhysics();
	std::function<void ()> m_onHitGround;
private:
    void init(Camera * cloneObj);
    bool m_enableFPSFeature;
    float m_maxFallSpeed;
    float m_distToside;
    bool m_isEnableGravity;
    vec3 m_speed;
    vec3 m_rotateSpeed;
    int m_forward;
    int m_slide;
	int m_up;
    vec3 m_oldPosition;
    bool m_isFirstLoop;
    float m_verticalSpeed;
    float m_gravity;
    float distToGround;
    float m_distToFront;
    bool m_isStopUpdate;
    int collisionRecursionDepth{};
    ColliderEllipsoid * collisionPackage;
    float offsetToCentre;
    bool m_isMoving{};
	btKinematicCharacterController * m_character;
	btPairCachingGhostObject * m_ghost2;
	float m_capsuleHigh;
	bool m_isOpenPhysics;
	void onHitGroundImp();
public:
	bool isIsOpenPhysics() const;
	void setIsOpenPhysics(const bool isOpenPhysics);
};


} // namespace tzw

#endif // TZW_FPSCAMERA_H
