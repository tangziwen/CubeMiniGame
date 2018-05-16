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
	m_fov = fov;
	m_aspect = aspect;
	m_near = near;
	m_far = far;
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

Matrix44 lookTo(vec3 dir, vec3 up)
{
	Matrix44 rotateInv;
	Matrix44 translateInv;
	rotateInv.setToIdentity();
	translateInv.setToIdentity();
	auto data = rotateInv.data();

	//vec3 const f((center).normalized());
	//vec3 const s(vec3::CrossProduct(f, up).normalized());
	//vec3 const u(vec3::CrossProduct(s, f));

	vec3 N = dir;
	N.normalize();
	vec3 U = up;
	U = vec3::CrossProduct(U, N);
	U.normalize();
	vec3 V = vec3::CrossProduct(N, U);

	data[0] = U.x;
	data[1] = U.y;
	data[2] = U.z;
	data[3] = 0.0f;

	data[4] = V.x;
	data[5] = V.y;
	data[6] = V.z;
	data[7] = 0.0f;

	data[8] = N.x;
	data[9] = N.y;
	data[10] = N.z;
	data[11] = 0.0f;

	return rotateInv;
}
static Matrix44 targetTo(vec3 eye, vec3 target, vec3 up)
{
	Matrix44 mat;
	auto out = mat.data();
  float eyex = eye.x,
      eyey = eye.y,
      eyez = eye.z,
      upx = up.x,
      upy = up.y,
      upz = up.z;

  float z0 = eyex - target.x,
      z1 = eyey - target.y,
      z2 = eyez - target.z;

  float len = z0*z0 + z1*z1 + z2*z2;
  if (len > 0) {
    len = 1 / sqrt(len);
    z0 *= len;
    z1 *= len;
    z2 *= len;
  }

  float x0 = upy * z2 - upz * z1,
      x1 = upz * z0 - upx * z2,
      x2 = upx * z1 - upy * z0;

  len = x0*x0 + x1*x1 + x2*x2;
  if (len > 0) {
    len = 1 / sqrt(len);
    x0 *= len;
    x1 *= len;
    x2 *= len;
  }

	out[0] = x0;
	out[1] = x1;
	out[2] = x2;
	out[3] = 0;
	out[4] = z1 * x2 - z2 * x1;
	out[5] = z2 * x0 - z0 * x2;
	out[6] = z0 * x1 - z1 * x0;
	out[7] = 0;
	out[8] = z0;
	out[9] = z1;
	out[10] = z2;
	out[11] = 0;
	out[12] = eyex;
	out[13] = eyey;
	out[14] = eyez;
	out[15] = 1;
	return mat;
}

void Camera::lookAt(vec3 targetPos, vec3 upFrame)
{
	auto rotateM = targetTo(m_pos, targetPos, upFrame);
    Quaternion q;
	auto m = rotateM.data();
    q.fromAxises(vec3(m[0], m[1], m[2]),vec3(m[4], m[5], m[6]),vec3(m[8], m[9],m[10]));
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
	cacheTransform();
	updateFrustum();
	for (auto child : m_children)
	{
		child->reCache();
	}
	//将重置缓存标记
	setNeedToUpdate(false); 
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

	void Camera::getPerspectInfo(float* fov, float* aspect, float* near, float* far)
	{
	(*fov) = m_fov;
	(*aspect) = m_aspect;
	(*near) = m_near;
	(*far) = m_far;
	}
} // namespace tzw

