#ifndef TZW_DRAWABLE2D_H
#define TZW_DRAWABLE2D_H
#include "Drawable.h"
#include "../Math/vec2.h"
namespace tzw {

class Drawable2D : public Drawable
{
public:
    Drawable2D();

    virtual vec2 getContentSize() const;
    virtual void setContentSize(const vec2 &getContentSize);

    virtual vec2 anchorPoint() const;
    virtual void setAnchorPoint(const vec2 &anchorPoint);

	Matrix44 getLocalTransform() override;
    virtual void setUniformColor(const tzw::vec4 &color);
    virtual void setUniformColor(const tzw::vec3 &color);
    virtual vec4 getUniformColor();
    virtual void setAlpha(float alphaValue);
	void setIsVisible(bool isDrawable) override;
	void setUpTransFormation(TransformationInfo &info) override;
protected:
    vec4 m_uniformColor;
    vec2 m_contentSize;
    vec2 m_anchorPoint;
    vec2 m_anchorPointInPoints;
};

} // namespace tzw

#endif // TZW_DRAWABLE2D_H
