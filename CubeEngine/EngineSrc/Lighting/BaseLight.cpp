#include "BaseLight.h"

namespace tzw {

BaseLight::BaseLight()
{
    m_lightColor = vec3(1.0,1.0,1.0);
    m_intensity = 1.0f;
}

vec3 BaseLight::getLightColor() const
{
    return m_lightColor;
}

void BaseLight::setLightColor(const vec3 &color)
{
    m_lightColor = color;
}

float BaseLight::intensity() const
{
    return m_intensity;
}

void BaseLight::setIntensity(const float &intensity)
{
    m_intensity = intensity;
}

} // namespace tzw
