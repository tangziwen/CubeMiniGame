#include "BaseLight.h"

namespace tzw {

BaseLight::BaseLight()
{
    m_color = vec3(1.0,1.0,1.0);
    m_intensity = 1.0f;
}

vec3 BaseLight::color() const
{
    return m_color;
}

void BaseLight::setColor(const vec3 &color)
{
    m_color = color;
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
