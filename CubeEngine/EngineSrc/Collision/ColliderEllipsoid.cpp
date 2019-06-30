#include "ColliderEllipsoid.h"

namespace tzw {

ColliderEllipsoid::ColliderEllipsoid(): foundCollision(false), nearestDistance(0)
{
}

vec3 ColliderEllipsoid::toE(vec3 theVec)
{
    return theVec / eRadius;
}

} // namespace tzw
