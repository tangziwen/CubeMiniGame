#include "camera.h"
#include "utility.h"
#include "math.h"
#include "QDebug"
#include <QVector4D>
#include <QQuaternion>
float frustumSplitFactor [4] = {0.2,0.4,0.7,1};
Camera::Camera()
{
    this->setNodeType (NODE_TYPE_CAMERA);
    m_frustumDirty = true;
    m_isManuallyView = false;
}

void Camera::setPerspective(float fov, float aspect, float z_near, float z_far)
{
    m_projection.setToIdentity();
    m_projection.perspective(fov,aspect,z_near,z_far);
    m_fov = fov;
    m_aspect = aspect;
    m_ZFar = z_far;
    m_ZNear = z_near;
    m_FrustumAABB =  getProjectionAABB (m_projection);
    splitFrustum(4);
}

void Camera::setOrtho(float left,float right,float bottom,float top,float near,float far)
{
    m_projection.setToIdentity ();
    m_projection.ortho (left,right,bottom,top,near,far);
}

inline float clampf(float value, float min_inclusive, float max_inclusive)
{
    if (min_inclusive > max_inclusive) {
        std::swap(min_inclusive, max_inclusive);
    }
    return value < min_inclusive ? min_inclusive : value < max_inclusive? value : max_inclusive;
}


void Camera::lookAt(QVector3D lookAtPos, QVector3D up)
{
    QVector3D upv = up;
    upv.normalize();
    QVector3D zaxis =this->m_pos - lookAtPos ;
    zaxis.normalize();

    QVector3D xaxis = QVector3D::crossProduct (upv,zaxis);
    xaxis.normalize();

    auto  yaxis = QVector3D::crossProduct (zaxis, xaxis);
    yaxis.normalize();

    QMatrix4x4  rotation;
    auto data = rotation.data ();
    data[0] = xaxis.x();
    data[1] = xaxis.y();
    data[2] = xaxis.z();
    data[3] = 0;

    data[4] = yaxis.x();
    data[5] = yaxis.y();
    data[6] = yaxis.z();
    data[7] = 0;
    data[8] = zaxis.x();
    data[9] = zaxis.y();
    data[10] = zaxis.z();
    data[11] = 0;

    auto  quaternion = utility::RotateMatrix2Quaternion (rotation);

    float rotx = atan2f(2 * (quaternion.scalar() * quaternion.x() + quaternion.y() * quaternion.z()), 1 - 2 * (quaternion.x() * quaternion.x() + quaternion.y() * quaternion.y()));
    float roty = asin(clampf(2 * (quaternion.scalar ()* quaternion.y() - quaternion.z() * quaternion.x()) , -1.0f , 1.0f));
    float rotz = -atan2(2 * (quaternion.scalar() * quaternion.z() + quaternion.x() * quaternion.y()) , 1 - 2 * (quaternion.y() * quaternion.y() + quaternion.z() * quaternion.z()));

    setRotation (QVector3D(T_RADIANS_TO_DEGREES(rotx),T_RADIANS_TO_DEGREES(roty),T_RADIANS_TO_DEGREES(rotz)));
}


QMatrix4x4 Camera::getViewMatrix()
{
    auto viewMatrix = getModelTrans().inverted ();
    if(viewMatrix != m_viewMatrix && !m_isManuallyView)
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
    QMatrix4x4 mat = m_projection*(getModelTrans().inverted ());
    QVector4D result4 = mat.inverted () * src4;
    float w = result4.w ();
    QVector3D result3 = result4.toVector3D () / w;
    return result3;
}

QVector3D Camera::screenToWorldDir(QVector2D v)
{
    auto near = ScreenToWorld(QVector3D(v,-1));
    auto far = ScreenToWorld (QVector3D(v,1));
    auto result = far - near;
    return result.normalized ();
}

Ray Camera::screenToWorldRay(QVector2D v)
{
    auto near = ScreenToWorld(QVector3D(v,-1));
    auto dir = screenToWorldDir (v);
    return Ray(near,dir);
}

QVector3D Camera::worldToScreen(QVector3D v)
{
    QVector4D NDCpos = getProjection () * (getViewMatrix ()*QVector4D(v,1.0));
    auto result = NDCpos.toVector3D ();
    result /=NDCpos.w ();
    return result;
}

bool Camera::isVisibleInFrustum(const AABB *aabb)
{
    if (m_frustumDirty)
    {
        m_frustum.initFrustumFromCamera(this);
        m_frustumDirty = false;
    }
    return !m_frustum.isOutOfFrustum(*aabb);
}

void Camera::splitFrustum( int NumofSplits)
{
    auto pre_zfar = m_ZNear;
    for(int i = 0;i<NumofSplits;i++)
    {
        auto z_far = m_ZFar*frustumSplitFactor[i];
        m_splitFrust[i].perspective (m_fov,m_aspect,pre_zfar,z_far);
        m_splitFrustumAABB[i] =getProjectionAABB(m_splitFrust[i]);
        pre_zfar = z_far;
    }
}

float Camera::ZNear() const
{
    return m_ZNear;
}

void Camera::setZNear(float ZNear)
{
    m_ZNear = ZNear;
}

float Camera::ZFar() const
{
    return m_ZFar;
}

void Camera::setZFar(float ZFar)
{
    m_ZFar = ZFar;
}
float Camera::fov() const
{
    return m_fov;
}

void Camera::setFov(float fov)
{
    m_fov = fov;
}
float Camera::aspect() const
{
    return m_aspect;
}

void Camera::setAspect(float aspect)
{
    m_aspect = aspect;
}

AABB Camera::getSplitFrustumAABB(int index)
{
    return m_splitFrustumAABB[index];
}

AABB Camera::getProjectionAABB(QMatrix4x4 projectMatrix)
{
    projectMatrix =  projectMatrix.inverted ();
    QVector4D bl_near = QVector4D(-1,-1,-1,1);
    bl_near = projectMatrix*bl_near;
    bl_near = QVector4D(bl_near.toVector3D ()/bl_near.w (),1);
    QVector4D br_near = QVector4D(1,-1,-1,1);
    br_near = projectMatrix*br_near;
    br_near = QVector4D(br_near.toVector3D ()/br_near.w (),1);
    QVector4D tl_near = QVector4D(-1,1,-1,1);
    tl_near = projectMatrix*tl_near;
    tl_near = QVector4D(tl_near.toVector3D ()/tl_near.w (),1);
    QVector4D tr_near = QVector4D(1,1,-1,1);
    tr_near = projectMatrix*tr_near;
    tr_near = QVector4D(tr_near.toVector3D ()/tr_near.w (),1);

    QVector4D bl_far = QVector4D(-1,-1,1,1);
    bl_far = projectMatrix*bl_far;
    bl_far = QVector4D(bl_far.toVector3D ()/bl_far.w (),1);
    QVector4D br_far = QVector4D(1,-1,1,1);
    br_far = projectMatrix*br_far;
    br_far = QVector4D(br_far.toVector3D ()/br_far.w (),1);
    QVector4D tl_far = QVector4D(-1,1,1,1);
    tl_far = projectMatrix*tl_far;
    tl_far = QVector4D(tl_far.toVector3D ()/tl_far.w (),1);
    QVector4D tr_far = QVector4D(1,1,1,1);
    tr_far = projectMatrix*tr_far;
    tr_far = QVector4D(tr_far.toVector3D ()/tr_far.w (),1);

    AABB aabb;
    aabb.update (bl_near.toVector3D ());
    aabb.update (br_near.toVector3D ());
    aabb.update (tl_near.toVector3D ());
    aabb.update (tr_near.toVector3D ());
    aabb.update (bl_far.toVector3D ());
    aabb.update (br_far.toVector3D ());
    aabb.update (tl_far.toVector3D ());
    aabb.update (tr_far.toVector3D ());

    return aabb;
}
AABB Camera::FrustumAABB() const
{
    return m_FrustumAABB;
}

void Camera::setFrustumAABB(const AABB &FrustumAABB)
{
    m_FrustumAABB = FrustumAABB;
}

void Camera::setViewMatrix(QMatrix4x4 matrix)
{
    m_viewMatrix = matrix;
    m_isManuallyView = true;
}





