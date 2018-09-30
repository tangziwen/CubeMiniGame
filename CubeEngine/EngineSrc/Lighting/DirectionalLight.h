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
	void setTheta(float newTheta);
	void setPhi(float newPhi);
	void setAngle(float newTheta, float newPhi);
	float theta() const;
	float phi() const;
private:
	void getAngleFromDir();
	void getDirFromAngle();
    vec3 m_dir;
	float m_theta;
	float m_phi;
};

} // namespace tzw

#endif // TZW_DIRECTIONALLIGHT_H
