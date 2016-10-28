#ifndef TZW_SPHERE_H
#define TZW_SPHERE_H

#include "vec3.h"
namespace tzw {

class t_Sphere
{
public:
    t_Sphere();
    vec3 centre() const;
    void setCentre(const vec3 &centre);

    float radius() const;
    void setRadius(float radius);

    bool intersectWithTriangle(vec3 v1, vec3 v2, vec3 v3, vec3 & hitPoint) const;

private:
    vec3 m_centre;
    float m_radius;
};

} // namespace tzw

#endif // TZW_SPHERE_H
