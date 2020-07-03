#pragma once

#include "../Base/Node.h"
#include "BaseLight.h"
#include "Interface/Drawable3D.h"
namespace tzw {

class PointLight : public Drawable3D, public BaseLight
{
public:
    PointLight();
	void tick(float dt);
	float getRadius() const;
	void setRadius(const float radius);
protected:
	float m_radius;
};

} // namespace tzw
