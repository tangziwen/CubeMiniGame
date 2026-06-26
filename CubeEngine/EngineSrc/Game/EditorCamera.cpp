#include "EditorCamera.h"

#include "EngineSrc/Engine/Engine.h"
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
	constexpr float kFakeFocusPlaneY = 0.0f;
	constexpr float kMinFocusDistance = 2.0f;
	constexpr float kMaxFocusDistance = 1000.0f;
	constexpr float kFocusPlaneMinT = 0.1f;
	constexpr float kOrbitStartDownAmount = 0.18f;
	constexpr float kOrbitFullDownAmount = 0.45f;
	constexpr float kWheelFallbackStep = 5.0f;
	constexpr float kDegreeToRadian = 3.14159265359f / 180.0f;

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
	setLocalPriority(-1);
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
	case TZW_KEY_T:
		if (m_mode == Mode::CityBuilder)
		{
			m_pitchRotate = -1;
		}
		break;
	case TZW_KEY_G:
		if (m_mode == Mode::CityBuilder)
		{
			m_pitchRotate = 1;
		}
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
	case TZW_KEY_T:
		if (m_pitchRotate < 0)
			m_pitchRotate = 0;
		break;
	case TZW_KEY_G:
		if (m_pitchRotate > 0)
			m_pitchRotate = 0;
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

bool EditorCamera::onScroll(vec2 offset)
{
	if (m_mode != Mode::CityBuilder
		|| isMouseInputBlocked()
		|| g_GetCurrScene()->defaultCamera() != this)
	{
		return false;
	}

	if (std::abs(offset.getY()) <= 0.0001f)
	{
		return false;
	}

	if (!m_cityBuilderFocusValid)
	{
		refreshFocusFromCenterPlane();
	}

	if (m_cityBuilderFocusValid)
	{
		m_cityBuilderFocusDistance *= std::pow(m_zoomSpeed, offset.getY());
		m_cityBuilderFocusDistance = std::clamp(
			m_cityBuilderFocusDistance,
			kMinFocusDistance,
			kMaxFocusDistance);
		placeOnFocusArm(1.0f);
	}
	else
	{
		setPos(getPos() + getForward() * (offset.getY() * kWheelFallbackStep));
	}
	return true;
}

void EditorCamera::logicUpdate(float dt)
{
	if (g_GetCurrScene()->defaultCamera() != this)
		return;

	if (m_mode == Mode::CityBuilder)
	{
		updateCityBuilderRotation(dt);
	}
	else if (m_yawRotate != 0)
	{
		m_yaw += static_cast<float>(m_yawRotate) * m_rotateSpeed * dt;
		applyEulerRotation();
	}

	vec3 movement(0.0f, 0.0f, 0.0f);
	if (m_forward != 0)
	{
		movement += (m_mode == Mode::CityBuilder ? yawForward() : projectedForward()) * static_cast<float>(m_forward);
	}
	if (m_slide != 0)
	{
		movement += (m_mode == Mode::CityBuilder ? yawRight() : projectedRight()) * static_cast<float>(m_slide);
	}

	if (movement.squaredLength() > kMinProjectedLengthSq)
	{
		movement.normalize();
		setPos(getPos() + movement * (m_moveSpeed * dt));
		if (m_mode == Mode::CityBuilder && m_cityBuilderFocusValid)
		{
			m_cityBuilderFocus += movement * (m_moveSpeed * dt);
		}
		m_isMoving = true;
	}
	else
	{
		m_isMoving = false;
	}
}

void EditorCamera::setRotateQ(const Quaternion& rotateQ)
{
	Node::setRotateQ(rotateQ);
	syncEulerFromRotation();
}

void EditorCamera::setMode(Mode mode)
{
	if (m_mode == mode)
		return;

	m_mode = mode;
	m_forward = 0;
	m_slide = 0;
	m_yawRotate = 0;
	m_pitchRotate = 0;
	syncEulerFromRotation();
}

EditorCamera::Mode EditorCamera::mode() const
{
	return m_mode;
}

void EditorCamera::setCityBuilderFocus(vec3 focusWorld)
{
	m_cityBuilderFocus = focusWorld;
	m_cityBuilderFocusDistance = std::clamp(
		getPos().distance(focusWorld),
		kMinFocusDistance,
		kMaxFocusDistance);
	m_cityBuilderFocusValid = true;
}

void EditorCamera::clearCityBuilderFocus()
{
	m_cityBuilderFocusValid = false;
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
	EventMgr::shared()->addStandaloneEventListener(this);
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

void EditorCamera::updateCityBuilderRotation(float dt)
{
	if (m_yawRotate == 0 && m_pitchRotate == 0)
	{
		return;
	}

	if (!m_cityBuilderFocusValid)
	{
		refreshFocusFromCenterPlane();
	}

	m_yaw += static_cast<float>(m_yawRotate) * m_rotateSpeed * dt;
	m_pitch = std::clamp(
		m_pitch + static_cast<float>(m_pitchRotate) * m_rotateSpeed * dt,
		kMinPitch,
		kMaxPitch);
	applyEulerRotation();

	if (m_cityBuilderFocusValid)
	{
		placeOnFocusArm(cityBuilderOrbitWeight());
	}
}

bool EditorCamera::refreshFocusFromCenterPlane()
{
	vec2 winSize = Engine::shared()->winSize();
	vec3 nearPt = unproject(vec3(winSize.x * 0.5f, winSize.y * 0.5f, 0.0f));
	vec3 farPt = unproject(vec3(winSize.x * 0.5f, winSize.y * 0.5f, 1.0f));
	vec3 dir = farPt - nearPt;
	if (dir.squaredLength() <= kMinProjectedLengthSq)
	{
		return false;
	}
	dir.normalize();
	if (dir.y >= -0.0001f)
	{
		return false;
	}

	const float t = (kFakeFocusPlaneY - getPos().y) / dir.y;
	if (t < kFocusPlaneMinT || t > kMaxFocusDistance)
	{
		return false;
	}

	setCityBuilderFocus(getPos() + dir * t);
	return true;
}

void EditorCamera::placeOnFocusArm(float weight)
{
	if (!m_cityBuilderFocusValid || weight <= 0.0f)
	{
		return;
	}

	weight = std::clamp(weight, 0.0f, 1.0f);
	const vec3 desiredPos = m_cityBuilderFocus - getForward() * m_cityBuilderFocusDistance;
	const vec3 currentPos = getPos();
	setPos(currentPos + (desiredPos - currentPos) * weight);
}

float EditorCamera::cityBuilderOrbitWeight()
{
	const float downAmount = std::clamp(-getForward().y, 0.0f, 1.0f);
	return std::clamp(
		(downAmount - kOrbitStartDownAmount) / (kOrbitFullDownAmount - kOrbitStartDownAmount),
		0.0f,
		1.0f);
}

vec3 EditorCamera::projectedForward() const
{
	return projectToXZ(const_cast<EditorCamera*>(this)->getForward());
}

vec3 EditorCamera::projectedRight() const
{
	return projectToXZ(const_cast<EditorCamera*>(this)->getTransform().right());
}

vec3 EditorCamera::yawForward() const
{
	const float yawRad = m_yaw * kDegreeToRadian;
	return vec3(-std::sin(yawRad), 0.0f, -std::cos(yawRad));
}

vec3 EditorCamera::yawRight() const
{
	const float yawRad = m_yaw * kDegreeToRadian;
	return vec3(std::cos(yawRad), 0.0f, -std::sin(yawRad));
}

} // namespace tzw
