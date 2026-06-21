#ifndef TZW_EDITORCAMERA_H
#define TZW_EDITORCAMERA_H

#include "EngineSrc/Base/Camera.h"
#include "EngineSrc/Event/Event.h"

namespace tzw {

class EditorCamera : public Camera, public EventListener
{
public:
	enum class Mode
	{
		Free,
		CityBuilder,
	};

	EditorCamera();
	static EditorCamera* create(Camera* cloneObj);

	bool onKeyPress(int keyCode) override;
	bool onKeyRelease(int keyCode) override;
	bool onMouseRelease(int button, vec2 pos) override;
	bool onMousePress(int button, vec2 pos) override;
	bool onMouseMove(vec2 pos) override;
	bool onScroll(vec2 offset) override;
	void logicUpdate(float dt) override;
	void setRotateQ(const Quaternion& rotateQ) override;

	void setMode(Mode mode);
	Mode mode() const;
	void setCityBuilderFocus(vec3 focusWorld);
	void clearCityBuilderFocus();
	void setMoveSpeed(float speed);
	float moveSpeed() const;
	void setRotateSpeed(float speed);
	float rotateSpeed() const;
	bool isMoving() const;

	static void setInputBlocked(bool blocked);
	static bool isInputBlocked();
	static void setMouseInputBlocked(bool blocked);
	static bool isMouseInputBlocked();

private:
	void init(Camera* cloneObj);
	void syncEulerFromRotation();
	void applyEulerRotation();
	void updateCityBuilderRotation(float dt);
	bool refreshFocusFromCenterPlane();
	void placeOnFocusArm(float weight);
	float cityBuilderOrbitWeight();
	vec3 projectedForward() const;
	vec3 projectedRight() const;
	vec3 yawForward() const;
	vec3 yawRight() const;

	Mode m_mode = Mode::CityBuilder;
	float m_moveSpeed = 10.0f;
	float m_rotateSpeed = 90.0f;
	float m_zoomSpeed = 0.85f;
	int m_forward = 0;
	int m_slide = 0;
	int m_yawRotate = 0;
	int m_pitchRotate = 0;
	bool m_isMoving = false;
	bool m_isRotating = false;
	bool m_isFirstMouseMove = true;
	bool m_cityBuilderFocusValid = false;
	vec2 m_oldMousePos;
	vec3 m_cityBuilderFocus;
	float m_cityBuilderFocusDistance = 10.0f;
	float m_yaw = 0.0f;
	float m_pitch = 0.0f;
	static bool s_inputBlocked;
	static bool s_mouseInputBlocked;
};

} // namespace tzw

#endif // TZW_EDITORCAMERA_H
