#ifndef TZW_COLLISIONUTILITY_H
#define TZW_COLLISIONUTILITY_H

#include "ColliderEllipsoid.h"
namespace tzw {

class CollisionUtility
{
public:
    CollisionUtility();
    static void checkTriangle(ColliderEllipsoid* colPackage, const vec3& p1, const vec3& p2, const vec3& p3);
    static bool getLowestRoot(float a, float b, float c, float maxR,float* root);
    static bool checkPointInTriangle(const vec3& point,
                                     const vec3& pa,const vec3& pb, const vec3& pc);
};

} // namespace tzw

#endif // TZW_COLLISIONUTILITY_H
