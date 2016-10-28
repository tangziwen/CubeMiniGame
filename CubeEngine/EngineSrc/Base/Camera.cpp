#include "Camera.h"
#include "External/TUtility/TUtility.h"
#include "EngineSrc/Engine/Engine.h"
namespace tzw {

Camera::Camera()
    :m_useCustomFrustumUpdate(false)
{

}
/**
 * @brief Camera::CreatePerspective 创建一个透视投影的相机
 * @param aspect
 * @param near
 * @param far
 * @return
 */
Camera *Camera::CreatePerspective(float fov, float aspect, float thenear, float thefar)
{
    auto camera = new Camera();
    camera->setPerspective(fov,aspect,thenear, thefar);
    camera->m_frustum.initFrustumFromCamera(camera);
    return camera;
}
/**
 * @brief Camera::CreateOrtho 创建一个正交投影的相机
 * @param left
 * @param right
 * @param bottom
 * @param top
 * @param near
 * @param far
 * @return
 */
Camera *Camera::CreateOrtho(float left, float right, float bottom, float top, float near, float far)
{
    auto camera = new Camera();
    camera->setOrtho(left,right,bottom,top,near,far);
    camera->m_frustum.initFrustumFromCamera(camera);
    return camera;
}
/**
 * @brief Camera::setPerspective 设置相机的投影矩阵为透视
 * @param fov
 * @param aspect
 * @param near
 * @param far
 */
void Camera::setPerspective(float fov,float aspect,float near,float far)
{
    m_projection.perspective(fov,aspect,near,far);
}

/**
 * @brief Camera::setOrtho 设置相机的投影矩阵为正交
 * @param left
 * @param right
 * @param bottom
 * @param top
 * @param near
 * @param far
 */
void Camera::setOrtho(float left,float right,float bottom,float top,float near,float far)
{
    m_projection.ortho(left,right,bottom,top,near,far);
}

bool Camera::isOutOfFrustum(AABB aabb)
{
    return m_frustum.isOutOfFrustum(aabb);
}
/**
 * @brief Camera::projection 获取相机的投影矩阵
 * @return
 */
Matrix44 Camera::projection() const
{
    return m_projection;
}
/**
 * @brief Camera::setProjection 直接设置相机的投影矩阵
 * @param projection
 */
void Camera::setProjection(const Matrix44 &projection)
{
    m_projection = projection;
    m_frustum.initFrustumFromCamera(this);
}
/**
 * @brief Camera::getViewMatrix 获取视图矩阵
 * @return
 */
Matrix44 Camera::getViewMatrix()
{
    return getTransform().inverted();
}
/**
 * @brief Camera::getViewProjectionMatrix 获取视图投影矩阵，其值为 投影矩阵 * 视图矩阵
 * @return
 */
Matrix44 Camera::getViewProjectionMatrix()
{
    auto view = getViewMatrix();
    return m_projection * view ;
}

void Camera::lookAt(vec3 targetPos, vec3 upFrame)
{

    auto aixZ = (targetPos - m_pos);
    aixZ.normalize();
    auto aixX = vec3::CrossProduct(aixZ,upFrame);
    aixX.normalize();

    auto aixY = vec3::CrossProduct(aixX,aixZ);
    aixY.normalize();
    Quaternion q;
    q.fromAxises(aixX,aixY,-aixZ);
    //m_rotateQ = q;
    setRotateQ(q);
    reCache();
    //    float rotateX,rotateY,rotateZ;
    //    if(aixX.z != 1 || aixX.z != -1)
    //    {
    //        rotateY = - asin(aixX.z);
    //        rotateX = atan2(aixY.z/cos(rotateY),aixZ.z/cos(rotateY));
    //        rotateZ = atan2(aixX.y/cos(rotateY),aixX.x/cos(rotateY));
    //    }else
    //    {
    //        rotateZ = 0.0;
    //        if(aixX.z == -1)
    //        {
    //            rotateY = TbaseMath::PI / 2.0;
    //            rotateX = rotateZ + atan2(aixY.x,aixZ.x);
    //        }else
    //        {
    //            rotateY = -1 * TbaseMath::PI / 2.0;
    //            rotateX = -rotateZ + atan2(-aixY.x,-aixZ.x);
    //        }
    //    }
    //    setRotateE(vec3(TbaseMath::Radius2Ang(rotateX),TbaseMath::Radius2Ang(rotateY),TbaseMath::Radius2Ang(rotateZ)));
}

void Camera::logicUpdate(float dt)
{

}

void Camera::updateFrustum()
{
    if(getNeedToUpdate())
    {
        m_frustum.initFrustumFromCamera(this);
    }
}
bool Camera::getUseCustomFrustumUpdate() const
{
    return m_useCustomFrustumUpdate;
}

void Camera::setUseCustomFrustumUpdate(bool useCustomFrustumUpdate)
{
    m_useCustomFrustumUpdate = useCustomFrustumUpdate;
}

void Camera::reCache()
{
    Node::reCache();
    updateFrustum();
}

vec3 Camera::unproject(vec3 src)
{
    auto viewport = Engine::shared()->winSize();
    vec4 screen(src.x / viewport.x, src.y / viewport.y, src.z, 1.0f);
    screen.x = (screen.x * 2.0f - 1.0f);
    screen.y = (screen.y * 2.0f - 1.0f);
    screen.z = (screen.z * 2.0f - 1.0f);

    auto inverseMat = getViewProjectionMatrix().inverted();
    screen = inverseMat * screen;
    if (screen.w != 0.0f)
    {
        float w = screen.w;
        screen.x = (screen.x / w);
        screen.y = (screen.y / w);
        screen.z = (screen.z / w);
    }
    return vec3(screen.x,screen.y,screen.z);
}
} // namespace tzw

