#ifndef TZW_COLLIDERELLIPSOID_H
#define TZW_COLLIDERELLIPSOID_H

#include "../Math/vec3.h"
namespace tzw {

class ColliderEllipsoid
{
public:
    ColliderEllipsoid();

    vec3 eRadius;  // ellipsoid radius
    // Information about the move being requested: (in R3)
    vec3 R3Velocity;
    vec3 R3Position;
    // Information about the move being requested: (in eSpace)
    vec3 velocity;
    vec3 normalizedVelocity;
    vec3 basePoint;
    vec3 toE(vec3 theVec);
    // Hit information
    bool foundCollision;
    double nearestDistance;
    vec3 intersectionPoint;
};

} // namespace tzw

#endif // TZW_COLLIDERELLIPSOID_H
