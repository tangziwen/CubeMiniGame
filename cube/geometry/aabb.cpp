#include "aabb.h"
#include <qvector4d.h>
AABB::AABB()
{
    reset();
}

AABB::~AABB()
{

}

void AABB::update(QVector3D * vec,int num)
{
    for (ssize_t i = 0; i < num; i++)
    {
        // Leftmost point.
        if (vec[i].x() < m_min.x())
            m_min.setX(vec[i].x ());

        // Lowest point.
        if (vec[i].y() < m_min.y())
            m_min.setY (vec[i].y ());

        // Farthest point.
        if (vec[i].z() < m_min.z())
            m_min.setZ (vec[i].z ());

        // Rightmost point.
        if (vec[i].x() > m_max.x())
            m_max.setX(vec[i].x ());


        // Highest point.
        if (vec[i].y() > m_max.y())
            m_max.setY (vec[i].y ());

        // Nearest point.
        if (vec[i].z() > m_max.z())
            m_max.setZ (vec[i].z ());
    }
}

void AABB::transForm(QMatrix4x4 mat)
{

    QVector3D corners[8];
     // Near face, specified counter-clockwise
    // Left-top-front.
    corners[0] = QVector3D(m_min.x(), m_max.y(), m_max.z());
    // Left-bottom-front.
    corners[1] = QVector3D(m_min.x(), m_min.y(), m_max.z());
    // Right-bottom-front.
    corners[2] = QVector3D(m_max.x(), m_min.y(), m_max.z());
    // Right-top-front.
    corners[3] = QVector3D(m_max.x(), m_max.y(), m_max.z());

    // Far face, specified clockwise
    // Right-top-back.
    corners[4] = QVector3D(m_max.x(), m_max.y(), m_min.z());
    // Right-bottom-back.
    corners[5] = QVector3D(m_max.x(), m_min.y(), m_min.z());
    // Left-bottom-back.
    corners[6] = QVector3D(m_min.x(), m_min.y(), m_min.z());
    // Left-top-back.
    corners[7] = QVector3D(m_min.x(), m_max.y(), m_min.z());
    for(int i =0 ; i < 8 ; i++)
    {
        QVector4D result = mat*QVector4D(corners[i],1);
        corners[i] = result.toVector3D ();
    }
    reset();
    update(corners,8);
}

void AABB::reset()
{
    m_min = QVector3D(999,999,999);
    m_max = QVector3D(-999,-999,-999);
}

void AABB::merge(AABB box)
{
        // Calculate the new minimum point.
        m_min.setX (std::min(m_min.x(), box.min ().x()));
        m_min.setY (std::min(m_min.y(), box.min ().y()));
        m_min.setZ (std::min(m_min.z(), box.min ().z()));

        // Calculate the new maximum point.
        m_max.setX (std::max(m_max.x(), box.max().x()));
        m_max.setY(std::max(m_max.y(), box.max().y()));
        m_max.setZ(std::max(m_max.z(), box.max().z()));
}

QVector3D AABB::min() const
{
    return m_min;
}

void AABB::setMin(const QVector3D &min)
{
    m_min = min;
}
QVector3D AABB::max() const
{
    return m_max;
}

void AABB::setMax(const QVector3D &max)
{
    m_max = max;
}



