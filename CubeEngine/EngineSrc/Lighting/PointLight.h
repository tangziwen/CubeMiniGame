#pragma once

#include "../Base/Node.h"
#include "BaseLight.h"
namespace tzw {

class PointLight : public BaseLight
{
public:
    PointLight();
	void tick(float dt);
	float getRadius() const;
	void setRadius(const float radius);
	vec3 getPos() const;
	void setPos(const vec3& pos);
protected:
	float m_radius;
	vec3 m_pos;
};

} // namespace tzw
