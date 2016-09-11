#include "Ray.h"
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

vec3 Ray::intersectPlane(Plane p)
{
    auto point = p .getNormal ()*p.getDist ();
    auto r = vec3::DotProduct (p.getNormal (),(point - m_origin)) / vec3::DotProduct (p.getNormal (),m_direction);
    auto result = m_origin + m_direction*r;
    return result;
}

bool Ray::intersectAABB(AABB aabb, RayAABBSide *side, vec3 &hitPoint)const
{
    vec3 ptOnPlane;
    vec3 min = aabb.min ();
    vec3 max = aabb.max ();

    const vec3& origin = m_origin;
    const vec3& dir = m_direction;

    float t;

    if(origin.x>min.x && origin.y>min.y && origin.z>min.z
            && origin.x<max.x && origin.y<max.y && origin.z < max.z)
    {
        return false;
    }

    if (dir.x != 0.f)
    {
        if (dir.x > 0)
            t = (min.x - origin.x) / dir.x;
        else
            t = (max.x - origin.x) / dir.x;

        if (t > 0.f)
        {
            ptOnPlane = origin +  dir * t;

            if (min.y < ptOnPlane.y && ptOnPlane.y < max.y && min.z < ptOnPlane.z && ptOnPlane.z < max.z)
            {
                hitPoint = ptOnPlane;
                if(side)
                {
                    if(ptOnPlane.x<max.x)
                        (*side) = RayAABBSide::left;
                    else
                        (*side) = RayAABBSide::right;
                }
                return true;
            }
        }
    }

    if (dir.y != 0.f)
    {
        if (dir.y > 0)
            t = (min.y - origin.y) / dir.y;
        else
            t = (max.y - origin.y) / dir.y;

        if (t > 0.f)
        {
            ptOnPlane = origin +  dir * t;

            if (min.z < ptOnPlane.z && ptOnPlane.z < max.z && min.x < ptOnPlane.x && ptOnPlane.x < max.x)
            {
                hitPoint = ptOnPlane;
                if(side)
                {
                    if(ptOnPlane.y<max.y)
                        (*side) = RayAABBSide::down;
                    else
                        (*side) = RayAABBSide::up;
                }
                return true;
            }
        }
    }

    if (dir.z != 0.f)
    {
        if (dir.z > 0)
            t = (min.z - origin.z) / dir.z;
        else
            t = (max.z - origin.z) / dir.z;

        if (t > 0.f)
        {
            ptOnPlane = origin +  dir * t;

            if (min.x < ptOnPlane.x && ptOnPlane.x < max.x && min.y < ptOnPlane.y && ptOnPlane.y < max.y)
            {
                hitPoint = ptOnPlane;
                if(side)
                {
                    if(ptOnPlane.z<max.z)
                        (*side) = RayAABBSide::back;
                    else
                        (*side) = RayAABBSide::front;
                }
                return true;
            }
        }
    }
    return false;
}

bool Ray::intersectTriangle(const vec3 &v1, const vec3 &v2, const vec3 &v3, float *out) const
{
    vec3 D = m_direction;
    vec3 e1, e2;  //Edge1, Edge2
    vec3 P, Q, T;
    float det, inv_det, u, v;
    float t;

    //Find vectors for two edges sharing V1
    e1 = v2 - v1;
    e2 = v3 - v1;
    //Begin calculating determinant - also used to calculate u parameter
    P = vec3::CrossProduct(m_direction, e2);
    //if determinant is near zero, ray lies in plane of triangle or ray is parallel to plane of triangle
    det = vec3::DotProduct(e1, P);
    //NOT CULLING
    if(det > -EPSILON && det < EPSILON) return 0;
    inv_det = 1.f / det;

    //calculate distance from V1 to ray origin
    T = m_origin - v1;

    //Calculate u parameter and test bound
    u = vec3::DotProduct(T, P) * inv_det;
    //The intersection lies outside of the triangle
    if(u < 0.f || u > 1.f) return 0;

    //Prepare to test v parameter
    Q = vec3::CrossProduct(T, e1);

    //Calculate V parameter and test bound
    v = vec3::DotProduct(D, Q) * inv_det;
    //The intersection lies outside of the triangle
    if(v < 0.f || u + v  > 1.f) return 0;

    t = vec3::DotProduct(e2, Q) * inv_det;

    if(t > EPSILON) { //ray intersection
        if (out) *out = t;
        return true;
    }

    // No hit, no win
    return false;
}

} // namespace tzw

