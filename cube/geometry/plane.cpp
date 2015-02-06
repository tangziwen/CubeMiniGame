#include "plane.h"


Plane::Plane()
:
_normal(0.f, 0.f, 1.f),
_dist(0.f)
{

}

// create plane from tree point
Plane::Plane(const QVector3D& p1, const QVector3D& p2, const QVector3D& p3)
{
    initPlane(p1, p2, p3);
}

// create plane from normal and dist
Plane::Plane(const QVector3D& normal, float dist)
{
    initPlane(normal, dist);
}

// create plane from normal and a point on plane
Plane::Plane(const QVector3D& normal, const QVector3D& point)
{
    initPlane(normal, point);
}

void Plane::initPlane(const QVector3D& p1, const QVector3D& p2, const QVector3D& p3)
{
    QVector3D p21 = p2 - p1;
    QVector3D p32 = p3 - p2;
    _normal = QVector3D::crossProduct (p21, p32);
    _normal.normalize ();
    _dist = QVector3D::dotProduct (_normal,p1);
}

void Plane::initPlane(const QVector3D& normal, float dist)
{
    float oneOverLength = 1 / normal.length();
    _normal = normal * oneOverLength;
    _dist = dist * oneOverLength;
}

void Plane::initPlane(const QVector3D& normal, const QVector3D& point)
{
    _normal = normal;
    _normal.normalize();
    _dist =  QVector3D::dotProduct (_normal,point);
}

float Plane::dist2Plane(const QVector3D& p) const
{
    return QVector3D::dotProduct (_normal,p) - _dist;
}


PointSide Plane::getSide(const QVector3D& point) const
{
    float dist = dist2Plane(point);
    if (dist > 0)
        return PointSide::FRONT_PLANE;
    else if (dist < 0)
        return PointSide::BEHIND_PLANE;
    else
        return PointSide::IN_PLANE;
}
