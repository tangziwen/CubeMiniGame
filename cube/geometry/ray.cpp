#include "ray.h"

Ray::Ray(QVector3D origin,QVector3D direction)
{
    m_origin = origin;
    m_direction = direction;
}

Ray::~Ray()
{

}
QVector3D Ray::origin() const
{
    return m_origin;
}

void Ray::setOrigin(const QVector3D &origin)
{
    m_origin = origin;
}
QVector3D Ray::direction() const
{
    return m_direction;
}

void Ray::setDirection(const QVector3D &direction)
{
    m_direction = direction;
}

bool Ray::intersect(AABB aabb, RayAABBSide *side)
{

    QVector3D ptOnPlane;
    QVector3D min = aabb.min ();
    QVector3D max = aabb.max ();

    const QVector3D& origin = m_origin;
    const QVector3D& dir = m_direction;

    float t;

    if(origin.x ()>min.x () && origin.y()>min.y() && origin.z ()>min.z ()
            && origin.x()<max.x () && origin.y()<max.y () && origin.z () < max.z ())
    {
        return true;
    }

    if (dir.x() != 0.f)
    {
        if (dir.x() > 0)
            t = (min.x() - origin.x()) / dir.x();
        else
            t = (max.x() - origin.x()) / dir.x();

        if (t > 0.f)
        {
            ptOnPlane = origin + t * dir;

            if (min.y() < ptOnPlane.y() && ptOnPlane.y() < max.y() && min.z() < ptOnPlane.z() && ptOnPlane.z() < max.z())
            {
                if(side)
                {
                    if(ptOnPlane.x ()<max.x ())
                        (*side) = RayAABBSide::left;
                    else
                        (*side) = RayAABBSide::right;
                }
                return true;
            }
        }
    }

    if (dir.y() != 0.f)
    {
        if (dir.y() > 0)
            t = (min.y() - origin.y()) / dir.y();
        else
            t = (max.y() - origin.y()) / dir.y();

        if (t > 0.f)
        {
            ptOnPlane = origin + t * dir;

            if (min.z() < ptOnPlane.z() && ptOnPlane.z() < max.z() && min.x() < ptOnPlane.x() && ptOnPlane.x() < max.x())
            {
                if(side)
                {
                    if(ptOnPlane.y ()<max.y ())
                        (*side) = RayAABBSide::down;
                    else
                        (*side) = RayAABBSide::up;
                }
                return true;
            }
        }
    }

    if (dir.z() != 0.f)
    {
        if (dir.z() > 0)
            t = (min.z() - origin.z()) / dir.z();
        else
            t = (max.z() - origin.z()) / dir.z();

        if (t > 0.f)
        {
            ptOnPlane = origin + t * dir;

            if (min.x() < ptOnPlane.x() && ptOnPlane.x() < max.x() && min.y() < ptOnPlane.y() && ptOnPlane.y() < max.y())
            {
                if(side)
                {
                    if(ptOnPlane.z ()<max.z ())
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
