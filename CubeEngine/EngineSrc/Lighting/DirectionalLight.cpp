#include "DirectionalLight.h"

namespace tzw {

DirectionalLight::DirectionalLight()
{
    m_dir = vec3(-0.5,-0.5,0);
	m_intensity = 0.4;
}

vec3 DirectionalLight::dir() const
{
    return m_dir;
}

void DirectionalLight::setDir(const vec3 &dir)
{
    m_dir = dir;
}
} // namespace tzw
