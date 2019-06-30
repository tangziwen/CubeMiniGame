#include "t_Sphere.h"
#include "Ray.h"
namespace tzw {

t_Sphere::t_Sphere(): m_radius(0)
{
}

vec3 t_Sphere::centre() const
{
    return m_centre;
}

void t_Sphere::setCentre(const vec3 &centre)
{
    m_centre = centre;
}

float t_Sphere::radius() const
{
    return m_radius;
}

void t_Sphere::setRadius(float radius)
{
    m_radius = radius;
}

bool t_Sphere::intersectWithTriangle(vec3 v1, vec3 v2, vec3 v3, vec3 &hitPoint) const
{
    vec3 normal = vec3::CrossProduct(v2 - v1, v3 - v1);
    normal.normalize();
    Ray ray(m_centre,-normal);
    float t;
    auto isHit = ray.intersectTriangle(v1, v2, v3, &t);
    if(isHit)
    {
        if( t <= m_radius)
        {
            hitPoint = ray.origin() + ray.direction() * t;
            return true;
        }
        else
        {
            return false;
        }
    }
    return false;
}

} // namespace tzw
