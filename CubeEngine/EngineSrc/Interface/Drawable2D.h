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

    virtual void setAlpha(float alphaValue);
	void setIsVisible(bool isDrawable) override;
	void setUpTransFormation(TransformationInfo &info) override;
    virtual bool isOutOfScreen();
    void recacheAABB();
    virtual void onTransformChanged() override;
    virtual NodeType getNodeType();
protected:
    vec2 m_contentSize;
    vec2 m_anchorPoint;
    vec2 m_anchorPointInPoints;
    AABB m_localAABB;
    AABB m_worldAABBCache;
};



class HorizonalLayOutHelper
{
public:
    HorizonalLayOutHelper(Drawable2D* parent, vec2 base, float padding = 2.f);
    void add(Drawable2D * obj);
    void doLayout();
protected:
    std::vector<Drawable2D * > m_objList;
    Drawable2D * m_parent;
    vec2 m_origin = vec2(0,0);
    float m_padding;
};

class HorizonalLayOut: public Drawable2D
{
public:
    HorizonalLayOut();
    HorizonalLayOut(Node * parent);
    void add(Drawable2D * obj);
    void doLayout();
    void logicUpdate(float dt) override;
protected:
    Drawable2D * m_parent;
    float m_padding;
};
class VerticalLayOut: public Drawable2D
{
public:
    VerticalLayOut();
    VerticalLayOut(Node * parent);
    void add(Drawable2D * obj);
    void doLayout();
    void logicUpdate(float dt) override;
protected:
    Drawable2D * m_parent;
    float m_padding;
};
} // namespace tzw

#endif // TZW_DRAWABLE2D_H
