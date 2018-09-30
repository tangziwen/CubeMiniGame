#include "DirectionalLight.h"
#include <math.h>
namespace tzw {

DirectionalLight::DirectionalLight()
{
    m_dir = vec3(1.0f, -1.0f, 0);
	m_dir.normalize();
	m_intensity = 1.0;
	getAngleFromDir();
}

vec3 DirectionalLight::dir() const
{
    return m_dir;
}

void DirectionalLight::setDir(const vec3 &dir)
{
    m_dir = dir;
	getAngleFromDir();
}

void DirectionalLight::setTheta(float newTheta)
{
	m_theta = newTheta;
	getDirFromAngle();
}

void DirectionalLight::setPhi(float newPhi)
{
	m_phi = newPhi;
	getDirFromAngle();
}

void DirectionalLight::setAngle(float newTheta, float newPhi)
{
	m_theta = newTheta;
	m_phi = newPhi;
	getDirFromAngle();
}

	void DirectionalLight::getAngleFromDir()
{
	m_theta = acos(m_dir.z);
	m_phi = atan2(m_dir.y, m_dir.x);
}

void DirectionalLight::getDirFromAngle()
{
	m_dir.x = sin(m_theta) * cos(m_phi);
	m_dir.y = sin(m_theta) * sin(m_phi);
	m_dir.z = cos(m_theta);
}

	float DirectionalLight::theta() const
	{
		return m_theta;
	}

	float DirectionalLight::phi() const
	{
		return m_phi;
	}
} // namespace tzw
