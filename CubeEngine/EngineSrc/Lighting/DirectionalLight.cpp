#include "DirectionalLight.h"

namespace tzw {

DirectionalLight::DirectionalLight()
{
    m_dir = vec3(1.0f, -1.0f, 0);
	//m_dir.normalize();
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
