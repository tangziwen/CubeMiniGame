#include "camera.h"
#include "utility.h"
#include "math.h"
#include "QDebug"


static float PI_OVER_180 =(3.1415927 / 180.0);

Camera::Camera()
{
    this->setNodeType (NODE_TYPE_CAMERA);
}

void Camera::setPerspective(float fov, float aspect, float z_near, float z_far)
{
    m_projection.setToIdentity();
    m_projection.perspective(fov,aspect,z_near,z_far);
}

QMatrix4x4 Camera::getViewMatrix()
{
    return getModelTrans().inverted ();
}

QMatrix4x4 Camera::getProjection()
{
    return this->m_projection;
}
