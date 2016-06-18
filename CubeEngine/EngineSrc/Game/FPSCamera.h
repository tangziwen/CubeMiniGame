#ifndef TZW_FPSCAMERA_H
#define TZW_FPSCAMERA_H

#include "../Base/Camera.h"
#include "../Event/Event.h"
#include <functional>
namespace tzw {

class FPSCamera : public Camera,public Event
{
public:
    FPSCamera();
    static FPSCamera * create(Camera * cloneObj);
    virtual void onKeyPress(int keyCode);
    virtual void onKeyRelease(int keyCode);
    virtual void onMouseRelease(int button,vec2 pos);
    virtual void onMousePress(int button,vec2 pos);
    virtual void onMouseMove(vec2 pos);
    virtual void update(float dt);
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
    virtual void reCache();

private:
    bool m_enableFPSFeature;
    float m_maxFallSpeed;
    float m_distToside;
    bool m_isEnableGravity;
    vec3 m_speed;
    vec3 m_rotateSpeed;
    int m_forward;
    int m_slide;
    vec3 m_oldPosition;
    bool m_isFirstLoop;
    float gravity_speed;
    float m_gravity;
    float distToGround;
    bool m_isStopUpdate;
};

} // namespace tzw

#endif // TZW_FPSCAMERA_H
