#include "PointLight.h"
#include <math.h>


#include "CubeGame/Attachment.h"
#include "Utility/math/TbaseMath.h"

namespace tzw {

PointLight::PointLight()
{
	m_intensity = 5.0f;
	m_radius = 5.0f;
	m_pos = vec3(0, 0 , 0);
	m_color = vec3(1, 1, 1);
}

void PointLight::tick(float dt)
{

}

float PointLight::getRadius() const
{
	return m_radius;
}

void PointLight::setRadius(const float radius)
{
	m_radius = radius;
}

vec3 PointLight::getPos() const
{
	return m_pos;
}

void PointLight::setPos(const vec3& pos)
{
	m_pos = pos;
}
} // namespace tzw
