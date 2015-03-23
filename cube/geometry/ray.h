#ifndef RAY_H
#define RAY_H
#include <QVector3D>
#include "aabb.h"
#include "plane.h"
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
    Ray(QVector3D origin,QVector3D direction);
    ~Ray();
    QVector3D origin() const;
    void setOrigin(const QVector3D &origin);
    QVector3D direction() const;
    void setDirection(const QVector3D &direction);
    QVector3D intersectPlane(Plane p);
    bool intersectAABB(AABB  aabb, RayAABBSide * side);
private:
    QVector3D m_origin;
    QVector3D m_direction;
};

#endif // RAY_H
