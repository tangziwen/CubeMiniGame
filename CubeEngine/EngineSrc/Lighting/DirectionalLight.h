#ifndef TZW_DIRECTIONALLIGHT_H
#define TZW_DIRECTIONALLIGHT_H

#include "../Base/Node.h"
#include "BaseLight.h"
namespace tzw {

class DirectionalLight : public BaseLight
{
public:
    DirectionalLight();
    vec3 dir() const;
    void setDir(const vec3 &dir);
	void setLatitude(float newTheta);
	void setLongitude(float newPhi);
	void setAngle(float latitude, float longitude);
	float latitude() const;
	float longitude() const;
	void setTimeAndLongitude(float newTime, float newLongitude);
	float getTime();
	int getElapseLevel();
	void setElapseLevel(float newLevel);
	void tick(float dt);
private:
	void getAngleFromDir();
	void getDirFromAngle();
    vec3 m_dir;
	float m_latitude;
	float m_longitude;
	float m_time;
	int m_elapseLevel;
};

} // namespace tzw

#endif // TZW_DIRECTIONALLIGHT_H
