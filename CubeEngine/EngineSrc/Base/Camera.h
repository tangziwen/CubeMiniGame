#ifndef TZW_CAMERA_H
#define TZW_CAMERA_H

#include "Node.h"
#include "../Math/Frustum.h"
namespace tzw {
/**
 * @brief 该类定义了一个相机，用于模拟场景中的观察者
 *
 * 该类定义了一个相机，用于模拟场景中的观察者，其基本行为和普通的Node无异，只是在渲染会选取其变换的逆矩阵作为视图矩阵
 * 同时该类维护了一个矩阵用于保存投影变换
 */
class Camera : public Node
{
public:
    Camera();

    static Camera * CreatePerspective(float fov, float aspect, float thenear, float thefar);
    static Camera * CreateOrtho(float left, float right, float bottom, float top, float near, float far);

    void setPerspective(float fov, float aspect, float near, float far);
    void setOrtho(float left, float right, float bottom, float top, float near, float far);
    bool isOutOfFrustum(AABB aabb);
    Matrix44 projection() const;
    void setProjection(const Matrix44 &projection);
    Matrix44 getViewMatrix();
    Matrix44 getViewProjectionMatrix();
    void lookAt(vec3 targetPos,vec3 upFrame);
	void logicUpdate(float dt) override;
    void updateFrustum();
    bool getUseCustomFrustumUpdate() const;
    void setUseCustomFrustumUpdate(bool useCustomFrustumUpdate);
	void reCache() override;
    vec3 unproject(vec3 src);
	void getPerspectInfo(float * fov, float * aspect, float * near, float * far);
protected:
    Frustum m_frustum;
    bool m_useCustomFrustumUpdate;
	float m_fov, m_aspect, m_near, m_far;
private:

    Matrix44 m_projection;
};

} // namespace tzw

#endif // TZW_CAMERA_H
