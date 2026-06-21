#include "EditorCamera.h"

#include "EngineSrc/Event/EventMgr.h"
#include "EngineSrc/Scene/SceneMgr.h"

#include <algorithm>
#include <cmath>

namespace tzw {

bool EditorCamera::s_inputBlocked = false;
bool EditorCamera::s_mouseInputBlocked = false;

namespace
{
	constexpr float kMinProjectedLengthSq = 0.000001f;
	constexpr float kMinPitch = -85.0f;
	constexpr float kMaxPitch = 85.0f;

	vec3 projectToXZ(vec3 direction)
	{
		direction.y = 0.0f;
		if (direction.squaredLength() <= kMinProjectedLengthSq)
		{
			return vec3(0.0f, 0.0f, 0.0f);
		}
		direction.normalize();
		return direction;
	}
}

EditorCamera::EditorCamera()
{
	setLocalPiority(-1);
}

EditorCamera* EditorCamera::create(Camera* cloneObj)
{
	auto camera = new EditorCamera();
	camera->init(cloneObj);
	return camera;
}

bool EditorCamera::onKeyPress(int keyCode)
{
	if (isInputBlocked() || g_GetCurrScene()->defaultCamera() != this)
		return false;

	switch (keyCode)
	{
	case TZW_KEY_W:
		m_forward = 1;
		break;
	case TZW_KEY_S:
		m_forward = -1;
		break;
	case TZW_KEY_A:
		m_slide = -1;
		break;
	case TZW_KEY_D:
		m_slide = 1;
		break;
	case TZW_KEY_Q:
		m_yawRotate = 1;
		break;
	case TZW_KEY_E:
		m_yawRotate = -1;
		break;
	default:
		break;
	}
	return false;
}

bool EditorCamera::onKeyRelease(int keyCode)
{
	switch (keyCode)
	{
	case TZW_KEY_W:
		if (m_forward > 0)
			m_forward = 0;
		break;
	case TZW_KEY_S:
		if (m_forward < 0)
			m_forward = 0;
		break;
	case TZW_KEY_A:
		if (m_slide < 0)
			m_slide = 0;
		break;
	case TZW_KEY_D:
		if (m_slide > 0)
			m_slide = 0;
		break;
	case TZW_KEY_Q:
		if (m_yawRotate > 0)
			m_yawRotate = 0;
		break;
	case TZW_KEY_E:
		if (m_yawRotate < 0)
			m_yawRotate = 0;
		break;
	default:
		break;
	}
	return false;
}

bool EditorCamera::onMouseRelease(int button, vec2 pos)
{
	return false;
}

bool EditorCamera::onMousePress(int button, vec2 pos)
{
	return false;
}

bool EditorCamera::onMouseMove(vec2 pos)
{
	return false;
}

void EditorCamera::logicUpdate(float dt)
{
	if (g_GetCurrScene()->defaultCamera() != this)
		return;

	if (m_yawRotate != 0)
	{
		m_yaw += static_cast<float>(m_yawRotate) * m_rotateSpeed * dt;
		applyEulerRotation();
	}

	vec3 movement(0.0f, 0.0f, 0.0f);
	if (m_forward != 0)
	{
		movement += projectedForward() * static_cast<float>(m_forward);
	}
	if (m_slide != 0)
	{
		movement += projectedRight() * static_cast<float>(m_slide);
	}

	if (movement.squaredLength() > kMinProjectedLengthSq)
	{
		movement.normalize();
		setPos(getPos() + movement * (m_moveSpeed * dt));
		m_isMoving = true;
	}
	else
	{
		m_isMoving = false;
	}
}

void EditorCamera::setMoveSpeed(float speed)
{
	m_moveSpeed = std::max(speed, 0.0f);
}

float EditorCamera::moveSpeed() const
{
	return m_moveSpeed;
}

void EditorCamera::setRotateSpeed(float speed)
{
	m_rotateSpeed = std::max(speed, 0.0f);
}

float EditorCamera::rotateSpeed() const
{
	return m_rotateSpeed;
}

bool EditorCamera::isMoving() const
{
	return m_isMoving;
}

void EditorCamera::setInputBlocked(bool blocked)
{
	s_inputBlocked = blocked;
}

bool EditorCamera::isInputBlocked()
{
	return s_inputBlocked;
}

void EditorCamera::setMouseInputBlocked(bool blocked)
{
	s_mouseInputBlocked = blocked;
}

bool EditorCamera::isMouseInputBlocked()
{
	return s_mouseInputBlocked;
}

void EditorCamera::init(Camera* cloneObj)
{
	setProjection(cloneObj->projection());
	setPos(cloneObj->getPos());
	setRotateQ(cloneObj->getRotateQ());
	setScale(vec3(1.0f, 1.0f, 1.0f));
	cloneObj->getPerspectInfo(&m_fov, &m_aspect, &m_near, &m_far);
	m_width = cloneObj->getWidth();
	m_height = cloneObj->getHeight();
	cloneObj->getPixelOffset(m_offsetPixelX, m_offsetPixelY);
	setOffsetPixel(m_offsetPixelX, m_offsetPixelY);
	syncEulerFromRotation();
	EventMgr::shared()->addFixedPiorityListener(this);
}

void EditorCamera::syncEulerFromRotation()
{
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
	getRotateQ().toEulserAngel(&x, &y, &z);
	m_pitch = std::clamp(x, kMinPitch, kMaxPitch);
	m_yaw = y;
	applyEulerRotation();
}

void EditorCamera::applyEulerRotation()
{
	setRotateE(vec3(m_pitch, m_yaw, 0.0f));
}

vec3 EditorCamera::projectedForward() const
{
	return projectToXZ(const_cast<EditorCamera*>(this)->getForward());
}

vec3 EditorCamera::projectedRight() const
{
	return projectToXZ(const_cast<EditorCamera*>(this)->getTransform().right());
}

} // namespace tzw
