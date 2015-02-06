#ifndef CAMERA_H
#define CAMERA_H
#include <QMatrix4x4>
#include "base/node.h"
#include <QVector3D>
#include <geometry/aabb.h>
#include <base/frustum.h>
class Camera : public Node
{
public:
    Camera();
    void setPerspective(float fov ,float aspect,float z_near,float z_far);
    void setOrtho(float left, float right, float bottom, float top, float near, float far);
    QMatrix4x4 getViewMatrix();
    QMatrix4x4 getProjection();
    QVector3D ScreenToWorld(QVector3D vec);
    bool isVisibleInFrustum(const AABB* aabb);
private:
    Frustum m_frustum;
    bool m_frustumDirty;
    QMatrix4x4 m_viewMatrix;
    QMatrix4x4 m_projection;
};


#endif // CAMERA_H
