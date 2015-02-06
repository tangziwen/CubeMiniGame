#include "camera.h"
#include "utility.h"
#include "math.h"
#include "QDebug"
#include <QVector4D>

Camera::Camera()
{
    this->setNodeType (NODE_TYPE_CAMERA);
    m_frustumDirty = true;
}

void Camera::setPerspective(float fov, float aspect, float z_near, float z_far)
{
    m_projection.setToIdentity();
    m_projection.perspective(fov,aspect,z_near,z_far);
}

void Camera::setOrtho(float left,float right,float bottom,float top,float near,float far)
{
    m_projection.setToIdentity ();
    m_projection.ortho (left,right,bottom,top,near,far);
}

QMatrix4x4 Camera::getViewMatrix()
{
    auto viewMatrix = getModelTrans().inverted ();
    if(viewMatrix != m_viewMatrix)
    {
        m_frustumDirty = true;
        m_viewMatrix = viewMatrix;
    }
    return m_viewMatrix;
}

QMatrix4x4 Camera::getProjection()
{
    return this->m_projection;
}

QVector3D Camera::ScreenToWorld(QVector3D vec)
{
    QVector4D src4(vec,1);
    QMatrix4x4 mat = m_projection*getModelTrans().inverted ();
    QVector4D result4 = mat.inverted () * src4;
    float w = result4.w ();
    QVector3D result3 = result4.toVector3D () / w;
    return result3;
}

bool Camera::isVisibleInFrustum(const AABB *aabb)
{
    if (m_frustumDirty)
    {
        m_frustum.initFrustum(this);
        m_frustumDirty = false;
    }
    return !m_frustum.isOutOfFrustum(*aabb);
}
