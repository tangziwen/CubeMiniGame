#ifndef TZW_BASELIGHT_H
#define TZW_BASELIGHT_H

#include "../Math/vec3.h"
namespace tzw {

class BaseLight
{
public:
    BaseLight();
    vec3 getLightColor() const;
    void setLightColor(const vec3 &color);

    float intensity() const;
    void setIntensity(const float &intensity);

protected:
    vec3 m_lightColor;
    float m_intensity;
};

} // namespace tzw

#endif // TZW_BASELIGHT_H
