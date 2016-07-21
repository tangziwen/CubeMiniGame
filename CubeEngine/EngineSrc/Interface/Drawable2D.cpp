#include "Drawable2D.h"

namespace tzw {

Drawable2D::Drawable2D()
{
    m_contentSize = vec2(0,0);
    m_anchorPoint = vec2(0,0);
    m_anchorPointInPoints = vec2(0,0);
}

vec2 Drawable2D::getContentSize() const
{
    return m_contentSize;
}

void Drawable2D::setContentSize(const vec2 &contentSize)
{
    m_contentSize = contentSize;
    m_anchorPointInPoints = vec2(m_contentSize.x * m_anchorPoint.x,m_contentSize.y * m_anchorPoint.y);
    m_needToUpdate = true;
}

vec2 Drawable2D::anchorPoint() const
{
    return m_anchorPoint;
}

void Drawable2D::setAnchorPoint(const vec2 &anchorPoint)
{
    m_anchorPoint = anchorPoint;
    m_anchorPointInPoints = vec2(m_contentSize.x * m_anchorPoint.x,m_contentSize.y * m_anchorPoint.y);
    m_needToUpdate = true;
}

Matrix44 Drawable2D::getLocalTransform()
{
    auto mat = Matrix44();
    mat.setToIdentity();
    mat.translate(vec3(-m_anchorPointInPoints.x,-m_anchorPointInPoints.y,0));
    return Node::getLocalTransform()*mat;
}

} // namespace tzw
