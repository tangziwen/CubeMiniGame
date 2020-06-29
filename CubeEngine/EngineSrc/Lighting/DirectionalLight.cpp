#include "DirectionalLight.h"
#include <math.h>


#include "CubeGame/Attachment.h"
#include "Utility/math/TbaseMath.h"

namespace tzw {

DirectionalLight::DirectionalLight()
{
    m_dir = vec3(1.0f, -1.0f, 0);
	m_dir.normalize();
	m_intensity = 5.0f;
	getAngleFromDir();
	setTimeAndLongitude(8, 0);
	setElapseLevel(0);
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

void DirectionalLight::setLatitude(float newTheta)
{
	m_latitude = newTheta;
	getDirFromAngle();
}

void DirectionalLight::setLongitude(float newPhi)
{
	m_longitude = newPhi;
	getDirFromAngle();
}

void DirectionalLight::setAngle(float theLatitude, float theLongitude)
{
	m_latitude = theLatitude;
	m_longitude = theLongitude;
	getDirFromAngle();
}

float DirectionalLight::getTime()
{
	return m_time;
}

int DirectionalLight::getElapseLevel()
{
	return m_elapseLevel;
}

//0 constant time
//1 24X 1hour
//2 30X 48 min
//3 48X 30 min
//4 72X 20 min
//5 120X 12
//6 180X 8
//7 240X 6
//8 340X 6
void DirectionalLight::setElapseLevel(float newLevel)
{
	m_elapseLevel = newLevel;
}

void DirectionalLight::tick(float dt)
{
	float speed = 0.0f;
	switch(getElapseLevel())
	{
    case 0:
		speed = 0.0f;
		break;
    case 1:
		speed = 24.f * dt;
		break;
    case 2:
		speed = 30.f * dt;
		break;
    case 3:
		speed = 48.f * dt;
		break;
    case 4:
		speed = 72.f * dt;
		break;
    case 5:
		speed = 120.f * dt;
		break;
    case 6:
		speed = 180.f * dt;
		break;
    case 7:
		speed = 240.f * dt;
		break;
    case 8:
		speed = 340.f * dt;
		break;
	}
	setTimeAndLongitude(m_time + speed / 3600.f, m_longitude);
}

void DirectionalLight::getAngleFromDir()
{
	m_latitude = acos(m_dir.y);
	m_longitude = atan2(m_dir.z, m_dir.x);
}

void DirectionalLight::getDirFromAngle()
{
	m_dir.x = sin(m_latitude) * cos(m_longitude);
	m_dir.y = cos(m_latitude);
	m_dir.z = sin(m_latitude) * sin(m_longitude);
}

	float DirectionalLight::latitude() const
	{
		return m_latitude;
	}

	float DirectionalLight::longitude() const
	{
		return m_longitude;
	}

void DirectionalLight::setTimeAndLongitude(float newTime, float newLongitude)
{
	m_time = newTime;
	m_longitude = newLongitude;
	m_latitude = TbaseMath::Ang2Radius(m_time * 360.f / 24.f);
	getDirFromAngle();
}
} // namespace tzw
