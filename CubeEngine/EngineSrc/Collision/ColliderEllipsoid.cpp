#include "ColliderEllipsoid.h"

namespace tzw {

ColliderEllipsoid::ColliderEllipsoid()
{

}

vec3 ColliderEllipsoid::toE(vec3 theVec)
{
    return theVec / eRadius;
}

} // namespace tzw
