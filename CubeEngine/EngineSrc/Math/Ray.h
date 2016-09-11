#ifndef TZW_RAY_H
#define TZW_RAY_H

#include "Plane.h"
#include "AABB.h"

namespace tzw {

enum class RayAABBSide{
    up =0,
    down,
    left,
    right,
    front,
    back,
};

class Ray
{
public:
    Ray();
    Ray(vec3 origin,vec3 direction);
    ~Ray();
    vec3 origin() const;
    void setOrigin(const vec3 &origin);
    vec3 direction() const;
    void setDirection(const vec3 &direction);
    vec3 intersectPlane(Plane p);
    bool intersectAABB(AABB  aabb, RayAABBSide * side,vec3 &hitPoint) const;
    bool intersectTriangle(const vec3 & v1, const vec3 & v2, const vec3 & v3, float * out = nullptr) const;
private:
    vec3 m_origin;
    vec3 m_direction;
};

} // namespace tzw

#endif // TZW_RAY_H
