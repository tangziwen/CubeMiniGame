#include "SpotLight.h"
#include <math.h>


#include "CubeGame/Attachment.h"
#include "Utility/math/TbaseMath.h"

namespace tzw {

SpotLight::SpotLight()
{
    m_dir = vec3(1.0f, -1.0f, 0);
	m_dir.normalize();
	m_intensity = 5.0f;
}

vec3 SpotLight::dir() const
{
    return m_dir;
}

void SpotLight::setDir(const vec3 &dir)
{
    m_dir = dir;
}

void SpotLight::tick(float dt)
{
}

} // namespace tzw
