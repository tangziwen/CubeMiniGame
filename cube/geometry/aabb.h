#ifndef AABB_H
#define AABB_H
#include <QVector3D>
#include <qmatrix4x4.h>
class AABB
{
public:
    AABB();
    ~AABB();
    void update(QVector3D *vec, int num);
    void transForm(QMatrix4x4 mat);
    void reset();
    void merge(AABB box);
    QVector3D min() const;
    void setMin(const QVector3D &min);

    QVector3D max() const;
    void setMax(const QVector3D &max);

private:
    QVector3D m_min;
    QVector3D m_max;
};

#endif // AABB_H
