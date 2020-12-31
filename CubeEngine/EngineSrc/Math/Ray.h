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
    vec3 intersectPlane(Plane p)
    {
        auto point = p.getNormal() * p.getDist();
        auto r = vec3::DotProduct(p.getNormal(), (point - m_origin)) / vec3::DotProduct(p.getNormal(), m_direction);
        auto result = m_origin + m_direction * r;
        return result;
    }
    bool intersectAABB(AABB  aabb, RayAABBSide* side, vec3& hitPoint) const
    {
        float lowt = 0.0f;
        float t;
        bool hit = false;
        const vec3 min = aabb.min();
        const vec3 max = aabb.max();
        const vec3 rayorig = m_origin;
        const vec3 raydir = m_direction;

        // Check origin inside first
        if (m_origin.x>min.x && m_origin.y>min.y && m_origin.z>min.z
            && m_origin.x<max.x && m_origin.y<max.y && m_origin.z < max.z)
        {
            return false;
        }
        // Check each face in turn, only check closest 3
        // Min x
        if (rayorig.x <= min.x && raydir.x > 0)
        {
            t = (min.x - rayorig.x) / raydir.x;
            if (t >= 0)
            {
                // Substitute t back into ray and check bounds and dist
                hitPoint = rayorig + raydir * t;
                if (hitPoint.y >= min.y && hitPoint.y <= max.y &&
                    hitPoint.z >= min.z && hitPoint.z <= max.z &&
                    (!hit || t < lowt))
                {
                    hit = true;
                    lowt = t;
                    if (side)
                    {
                        (*side) = RayAABBSide::left;
                    }
                }
            }
        }
        // Max x
        if (rayorig.x >= max.x && raydir.x < 0)
        {
            t = (max.x - rayorig.x) / raydir.x;
            if (t >= 0)
            {
                // Substitute t back into ray and check bounds and dist
                hitPoint = rayorig + raydir * t;
                if (hitPoint.y >= min.y && hitPoint.y <= max.y &&
                    hitPoint.z >= min.z && hitPoint.z <= max.z &&
                    (!hit || t < lowt))
                {
                    hit = true;
                    lowt = t;
                    if (side)
                    {
                        (*side) = RayAABBSide::right;
                    }
                }
            }
        }
        // Min y
        if (rayorig.y <= min.y && raydir.y > 0)
        {
            t = (min.y - rayorig.y) / raydir.y;
            if (t >= 0)
            {
                // Substitute t back into ray and check bounds and dist
                hitPoint = rayorig + raydir * t;
                if (hitPoint.x >= min.x && hitPoint.x <= max.x &&
                    hitPoint.z >= min.z && hitPoint.z <= max.z &&
                    (!hit || t < lowt))
                {
                    hit = true;
                    lowt = t;
                    if (side)
                    {
                        (*side) = RayAABBSide::down;
                    }
                }
            }
        }
        // Max y
        if (rayorig.y >= max.y && raydir.y < 0)
        {
            t = (max.y - rayorig.y) / raydir.y;
            if


                (t >= 0)
            {
                // Substitute t back into ray and check bounds and dist
                hitPoint = rayorig + raydir * t;
                if (hitPoint.x >= min.x && hitPoint.x <= max.x &&
                    hitPoint.z >= min.z && hitPoint.z <= max.z &&
                    (!hit || t < lowt))
                {
                    hit = true;
                    lowt = t;
                    if (side)
                    {
                        (*side) = RayAABBSide::up;
                    }
                }
            }
        }
        // Min z
        if (rayorig.z <= min.z && raydir.z > 0)
        {
            t = (min.z - rayorig.z) / raydir.z;
            if (t >= 0)
            {
                // Substitute t back into ray and check bounds and dist
                hitPoint = rayorig + raydir * t;
                if (hitPoint.x >= min.x && hitPoint.x <= max.x &&
                    hitPoint.y >= min.y && hitPoint.y <= max.y &&
                    (!hit || t < lowt))
                {
                    hit = true;
                    lowt = t;
                    if (side)
                    {
                        (*side) = RayAABBSide::front;
                    }
                }
            }
        }
        // Max z
        if (rayorig.z >= max.z && raydir.z < 0)
        {
            t = (max.z - rayorig.z) / raydir.z;
            if (t >= 0)
            {
                // Substitute t back into ray and check bounds and dist
                hitPoint = rayorig + raydir * t;
                if (hitPoint.x >= min.x && hitPoint.x <= max.x &&
                    hitPoint.y >= min.y && hitPoint.y <= max.y &&
                    (!hit || t < lowt))
                {
                    hit = true;
                    lowt = t;
                    if (side)
                    {
                        (*side) = RayAABBSide::back;
                    }
                }
            }
        }

        hitPoint = m_origin + m_direction * lowt;

        return hit;
    }
    bool intersectAABB(AABB aabb, vec3& hitPoint) const
    {
        return intersectAABB(aabb, nullptr, hitPoint);
    }
    bool intersectTriangle(const vec3& v1, const vec3& v2, const vec3& v3, float* out = nullptr) const
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
        if (det > -std::numeric_limits<float>::epsilon() && det < std::numeric_limits<float>::epsilon()) return 0;
        inv_det = 1.f / det;

        //calculate distance from V1 to ray origin
        T = m_origin - v1;

        //Calculate u parameter and test bound
        u = vec3::DotProduct(T, P) * inv_det;
        //The intersection lies outside of the triangle
        if (u < 0.f || u > 1.f) return 0;

        //Prepare to test v parameter
        Q = vec3::CrossProduct(T, e1);

        //Calculate V parameter and test bound
        v = vec3::DotProduct(D, Q) * inv_det;
        //The intersection lies outside of the triangle
        if (v < 0.f || u + v  > 1.f) return 0;

        t = vec3::DotProduct(e2, Q) * inv_det;

        if (t > std::numeric_limits<float>::epsilon()) { //ray intersection
            if (out) *out = t;
            return true;
        }

        // No hit, no win
        return false;
    }
private:
    vec3 m_origin;
    vec3 m_direction;
};

} // namespace tzw

#endif // TZW_RAY_H
