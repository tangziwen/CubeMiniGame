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
	m_lightColor = vec3(1, 1, 1);
	m_drawableFlag = static_cast<uint32_t>(DrawableFlag::PointLight);
	m_localAABB.setMin(vec3(-m_radius, -m_radius,- m_radius));
	m_localAABB.setMax(vec3(m_radius, m_radius, m_radius));
	Drawable3D::reCache();
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
	m_localAABB.setMin(vec3(-m_radius, -m_radius,- m_radius));
	m_localAABB.setMax(vec3(m_radius, m_radius, m_radius));
	reCache();
}

} // namespace tzw
