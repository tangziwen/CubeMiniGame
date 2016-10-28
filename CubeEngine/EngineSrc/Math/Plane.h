#ifndef TZW_PLANE_H
#define TZW_PLANE_H
#include "vec3.h"

namespace tzw {
enum class PointSide
{
    IN_PLANE,
    FRONT_PLANE,
    BEHIND_PLANE,
};

class Plane
{
public:
    /**
    * create plane from tree point.
    */
    Plane(const vec3& p1, const vec3& p2, const vec3& p3);

    /**
    * create plane from normal and dist.
    */
    Plane(const vec3& normal, float dist);

    /**
    * create plane from normal and a point on plane.
    */
    Plane(const vec3& normal, const vec3& point);

    /**
     * create a default plan whose normal is (0, 0, 1), and _dist is 0, xoy plan in fact.
     */
    Plane();

    /**
    * init plane from tree point.
    */
    void initPlane(const vec3& p1, const vec3& p2, const vec3& p3);

    /**
    * init plane from normal and dist.
    */
    void initPlane(const vec3& normal, float dist);

    /**
    * init plane from normal and a point on plane.
    */
    void initPlane(const vec3& normal, const vec3& point);

    /**
    * dist to plane, > 0 normal direction
    */
    float dist2Plane(const vec3& p) const;

    /**
    * Gets the plane's normal.
    */
    const vec3& getNormal() const { return _normal; }

    /**
    * Gets the plane's distance to the origin along its normal.
    */
    float getDist() const  { return _dist; }

    /**
    * Return the side where the point is.
    */
    PointSide getSide(const vec3& point) const;

    vec3 projectTo(const vec3& point) const;

    bool isFrontFacingTo(const vec3& direction) const;

protected:
    vec3 _normal;
    float _dist;
};
} // namespace tzw

#endif // TZW_PLANE_H
