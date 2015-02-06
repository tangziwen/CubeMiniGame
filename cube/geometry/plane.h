#ifndef PLANE_H
#define PLANE_H
#include <QVector3D>
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
    Plane(const QVector3D& p1, const QVector3D& p2, const QVector3D& p3);

    /**
    * create plane from normal and dist.
    */
    Plane(const QVector3D& normal, float dist);

    /**
    * create plane from normal and a point on plane.
    */
    Plane(const QVector3D& normal, const QVector3D& point);

    /**
     * create a default plan whose normal is (0, 0, 1), and _dist is 0, xoy plan in fact.
     */
    Plane();

    /**
    * init plane from tree point.
    */
    void initPlane(const QVector3D& p1, const QVector3D& p2, const QVector3D& p3);

    /**
    * init plane from normal and dist.
    */
    void initPlane(const QVector3D& normal, float dist);

    /**
    * init plane from normal and a point on plane.
    */
    void initPlane(const QVector3D& normal, const QVector3D& point);

    /**
    * dist to plane, > 0 normal direction
    */
    float dist2Plane(const QVector3D& p) const;

    /**
    * Gets the plane's normal.
    */
    const QVector3D& getNormal() const { return _normal; }

    /**
    * Gets the plane's distance to the origin along its normal.
    */
    float getDist() const  { return _dist; }

    /**
    * Return the side where the point is.
    */
    PointSide getSide(const QVector3D& point) const;

protected:
    QVector3D _normal;
    float _dist;
};


#endif // PLANE_H
