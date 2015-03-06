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
    void splitFrustum(int NumofSplits);

    float ZNear() const;
    void setZNear(float ZNear);

    float ZFar() const;
    void setZFar(float ZFar);

    float fov() const;
    void setFov(float fov);

    float aspect() const;
    void setAspect(float aspect);
    AABB getSplitFrustumAABB(int index);
    AABB FrustumAABB() const;
    void setFrustumAABB(const AABB &FrustumAABB);

private:
    AABB getProjectionAABB(QMatrix4x4 projectMatrix);
    Frustum m_frustum;
    bool m_frustumDirty;
    QMatrix4x4 m_viewMatrix;
    QMatrix4x4 m_projection;
    float m_ZNear;
    float m_ZFar;
    float m_fov;
    float m_aspect;
    QMatrix4x4 m_splitFrust[4];
    Frustum m_splitFrustum[4];
    AABB m_splitFrustumAABB[4];
    AABB m_FrustumAABB;
};


#endif // CAMERA_H
