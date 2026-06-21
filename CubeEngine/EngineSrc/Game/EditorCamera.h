#ifndef TZW_EDITORCAMERA_H
#define TZW_EDITORCAMERA_H

#include "EngineSrc/Base/Camera.h"
#include "EngineSrc/Event/Event.h"

namespace tzw {

class EditorCamera : public Camera, public EventListener
{
public:
	EditorCamera();
	static EditorCamera* create(Camera* cloneObj);

	bool onKeyPress(int keyCode) override;
	bool onKeyRelease(int keyCode) override;
	bool onMouseRelease(int button, vec2 pos) override;
	bool onMousePress(int button, vec2 pos) override;
	bool onMouseMove(vec2 pos) override;
	void logicUpdate(float dt) override;

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
	vec3 projectedForward() const;
	vec3 projectedRight() const;

	float m_moveSpeed = 10.0f;
	float m_rotateSpeed = 90.0f;
	int m_forward = 0;
	int m_slide = 0;
	int m_yawRotate = 0;
	bool m_isMoving = false;
	bool m_isRotating = false;
	bool m_isFirstMouseMove = true;
	vec2 m_oldMousePos;
	float m_yaw = 0.0f;
	float m_pitch = 0.0f;
	static bool s_inputBlocked;
	static bool s_mouseInputBlocked;
};

} // namespace tzw

#endif // TZW_EDITORCAMERA_H
