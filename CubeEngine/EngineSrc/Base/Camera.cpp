#include "Camera.h"
#include "EngineSrc/Engine/Engine.h"
namespace tzw {

Camera::Camera()
	: m_useCustomFrustumUpdate(false), m_fov(0), m_aspect(0), m_near(0), m_far(0)
{
}


Camera *Camera::CreatePerspective(float fov, float width, float height, float thenear, float thefar)
{
    auto camera = new Camera();
    camera->setPerspective(fov,width / height,thenear, thefar);
    camera->m_width = width;
    camera->m_height = height;
    camera->m_frustum.initFrustumFromProjectMatrix(camera->getViewProjectionMatrix());
    return camera;
}

void Camera::setOffsetPixel(float x, float y)
{
    m_offsetPixelX = x;
    m_offsetPixelY = y;
    auto projection = m_projection.data();
	projection[2] = -2.0f * m_offsetPixelX / m_width;
	projection[6] = 2.0f * m_offsetPixelY / m_height;
}

Camera *Camera::CreateOrtho(float left, float right, float bottom, float top, float near, float far)
{
    auto camera = new Camera();
    camera->setOrtho(left,right,bottom,top,near,far);
    camera->m_frustum.initFrustumFromProjectMatrix(camera->getViewProjectionMatrix());
    return camera;
}

void Camera::setPerspective(float fov,float aspect,float near,float far)
{
    m_projection.perspective(fov,aspect,near,far);
	m_fov = fov;
	m_aspect = aspect;
	m_near = near;
	m_far = far;
}


void Camera::setOrtho(float left,float right,float bottom,float top,float near,float far)
{
    m_projection.ortho(left,right,bottom,top,near,far);
}

bool Camera::isOutOfFrustum(const AABB& aabb)
{
    return m_frustum.isOutOfFrustum(aabb);
}


Matrix44 Camera::projection() const
{
    return m_projection;
}

void Camera::setProjection(const Matrix44 &projection)
{
    m_projection = projection;
    m_frustum.initFrustumFromProjectMatrix(getViewProjectionMatrix());
}

Matrix44 Camera::getViewMatrix()
{
    return this->getTransform().inverted();
}

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
    up.normalize();

    vec3 zaxis;
    zaxis = eye - target;//vec3::subtract(eye, target, &zaxis);
    zaxis.normalize();

    vec3 xaxis;
    xaxis = vec3::CrossProduct(up, zaxis);
    xaxis.normalize();

    vec3 yaxis;
    yaxis = vec3::CrossProduct(zaxis, xaxis);
    yaxis.normalize();

    out[0] = xaxis.x;
    out[1] = xaxis.y;
    out[2] = xaxis.z;
    out[3] = 0.0f;

    out[4] = yaxis.x;
    out[5] = yaxis.y;
    out[6] = yaxis.z;
    out[7] = 0.0f;

    out[8] = zaxis.x;
    out[9] = zaxis.y;
    out[10] = zaxis.z;
    out[11] = 0.0f;

    out[12] = eye.x;
    out[13] = eye.y;
    out[14] = eye.z;
    out[15] = 1.0f;
	return mat;
}

void Camera::lookAt(vec3 targetPos, vec3 upFrame)
{
	auto rotateM = targetTo(m_pos, targetPos, upFrame);
    Quaternion q;
	auto m = rotateM.data();
    q.fromRotationMatrix(&rotateM);
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
	if(m_useCustomFrustumUpdate) return;
    if(getNeedToUpdate())
    {
        m_frustum.initFrustumFromProjectMatrix(getViewProjectionMatrix());
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
	//no need to update
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

vec3 Camera::worldToScreen(vec3 worldPos)
{
    auto viewport = Engine::shared()->winSize();
    auto vp = getViewProjectionMatrix();
    vec4 screen = vp * vec4(worldPos, 1.0f);
    if (screen.w != 0.0f)
    {
        float w = screen.w;
        screen.x = (screen.x / w) * 0.5 + 0.5;
        screen.y = (screen.y / w) * 0.5 + 0.5;
        screen.z = (screen.z / w);
    }
    return vec3(screen.x * viewport.x,screen.y * viewport.y, screen.z);
}

void Camera::getPerspectInfo(float* fov, float* aspect, float* near, float* far)
	{
	(*fov) = m_fov;
	(*aspect) = m_aspect;
	(*near) = m_near;
	(*far) = m_far;
	}

} // namespace tzw

