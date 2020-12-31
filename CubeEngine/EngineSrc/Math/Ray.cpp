#include "Ray.h"
#include <algorithm>
#define EPSILON 0.000001
namespace tzw {

Ray::Ray()
{

}

Ray::Ray(vec3 origin, vec3 direction)
{
    m_origin = origin;
    m_direction = direction;
}

Ray::~Ray()
{

}
vec3 Ray::origin() const
{
    return m_origin;
}

void Ray::setOrigin(const vec3 &origin)
{
    m_origin = origin;
}
vec3 Ray::direction() const
{
    return m_direction;
}

void Ray::setDirection(const vec3 &direction)
{
    m_direction = direction;
}

struct AABBHitInfo
{
	vec3 hitPos;
	RayAABBSide side;
};

} // namespace tzw

