#ifndef TZW_FPSCAMERA_H
#define TZW_FPSCAMERA_H

#include "EngineSrc/Base/Camera.h"
#include "EngineSrc/Event/Event.h"
#include <functional>
#include "EngineSrc/Interface/Drawable3D.h"
#include "EngineSrc/Collision/ColliderEllipsoid.h"
namespace tzw {

class FPSCamera : public Camera,public EventListener
{
public:
    FPSCamera();
    static FPSCamera * create(Camera * cloneObj);
	bool onKeyPress(int keyCode) override;
	bool onKeyRelease(int keyCode) override;
	bool onMouseRelease(int button,vec2 pos) override;
	bool onMousePress(int button,vec2 pos) override;
	bool onMouseMove(vec2 pos) override;
	void logicUpdate(float dt) override;
    vec3 speed() const;
    void setSpeed(const vec3 &speed);

    vec3 rotateSpeed() const;
    void setRotateSpeed(const vec3 &rotateSpeed);

    float getDistToGround() const;
    void setDistToGround(float value);

    float getGravity() const;
    void setGravity(float gravity);

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

private:
    void init(Camera * cloneObj);
    void collideAndSlide(vec3 vel, vec3 gravity);
    vec3 collideWithWorld(const vec3& pos, const vec3& vel, bool needSlide = true);
    void checkCollision(ColliderEllipsoid * thePackage);
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
    int collisionRecursionDepth;
    ColliderEllipsoid * collisionPackage;
    float offsetToCentre;
    bool m_isMoving;
};

} // namespace tzw

#endif // TZW_FPSCAMERA_H
