#ifndef TZW_CAMERA_H
#define TZW_CAMERA_H

#include "Node.h"
#include "../Math/Frustum.h"
namespace tzw {
class Camera : public Node
{
public:
    Camera();

    static Camera * CreatePerspective(float fov, float width, float height, float thenear, float thefar);
    static Camera * CreateOrtho(float left, float right, float bottom, float top, float near, float far);

    void setPerspective(float fov, float aspect, float near, float far);
    void setOrtho(float left, float right, float bottom, float top, float near, float far);
    bool isOutOfFrustum(const AABB& aabb);
    Matrix44 projection() const;
    void setProjection(const Matrix44 &projection);
    void setOffsetPixel(float x, float y);
    Matrix44 getViewMatrix();
    Matrix44 getViewProjectionMatrix();
    void lookAt(vec3 targetPos,vec3 upFrame);
	void logicUpdate(float dt) override;
    void updateFrustum();
    bool getUseCustomFrustumUpdate() const;
    void setUseCustomFrustumUpdate(bool useCustomFrustumUpdate);
	void reCache() override;
    vec3 unproject(vec3 src);
	vec3 worldToScreen(vec3 worldPos);
	void getPerspectInfo(float * fov, float * aspect, float * near, float * far);
	float getFov() const {return m_fov;}
	float getNear() const {return m_near;}
	float getFar() const {return m_far;}
	float getAspect() const {return m_aspect;}
	float getWidth() const {return m_width;}
	float getHeight() const {return m_height;};
	void getPixelOffset(float & offsetX, float & offsetY) const
	{
		offsetX = m_offsetPixelX;
		offsetY = m_offsetPixelY;
	}
protected:
    Frustum m_frustum;
    bool m_useCustomFrustumUpdate;
	float m_fov, m_aspect, m_near, m_far,m_width, m_height,m_offsetPixelX,m_offsetPixelY;
private:

    Matrix44 m_projection;
};

} // namespace tzw

#endif // TZW_CAMERA_H
