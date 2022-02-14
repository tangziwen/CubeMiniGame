#include "Drawable2D.h"
#include "Event/EventMgr.h"
#include "Scene/SceneMgr.h"
#include "Engine/Engine.h"

namespace tzw {

Drawable2D::Drawable2D()
{
    m_material = nullptr;
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
    m_localAABB.setMin(vec3(0, 0, 0));
    m_localAABB.setMax(vec3(m_contentSize.x, m_contentSize.y, 0));
}

vec2 Drawable2D::anchorPoint() const
{
    return m_anchorPoint;
}


void Drawable2D::setUpTransFormation(TransformationInfo &info)
{
	auto currCam = g_GetCurrScene()->defaultGUICamera();
    info.m_projectMatrix = currCam->projection();
    info.m_viewMatrix = currCam->getViewMatrix();
    info.m_worldMatrix = getTransform();
}
bool Drawable2D::isOutOfScreen()
{
    vec2 screen = vec2(Engine::shared()->windowWidth(), Engine::shared()->windowHeight());
    vec3 aabb_min = m_worldAABBCache.min();
    vec3 aabb_max = m_worldAABBCache.max();
    return (aabb_min.x > screen.x) || (aabb_min.y > screen.y) || (aabb_max.x < 0) || (aabb_max.y < 0);
}

void Drawable2D::recacheAABB()
{
    if(getNeedToUpdate())
    {
        m_worldAABBCache = m_localAABB;
        m_worldAABBCache.transForm(getTransform());
    }
}

void Drawable2D::onTransformChanged()
{
    recacheAABB();
}

Node::NodeType Drawable2D::getNodeType()
{
    return NodeType::DrawableUI;
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
    mat.setTranslate(vec3(-m_anchorPointInPoints.x,-m_anchorPointInPoints.y,0));
    return Node::getLocalTransform()*mat;
}



void Drawable2D::setAlpha(float alphaValue)
{
    m_color.w = alphaValue;
}

void Drawable2D::setIsVisible(bool isDrawable)
{
	Drawable::setIsVisible(isDrawable);
	EventMgr::shared()->notifyListenerChange();
}

HorizonalLayOutHelper::HorizonalLayOutHelper(Drawable2D* parent, vec2 base, float padding)
    :m_parent(parent), m_origin(base), m_padding(padding)
{
}

void HorizonalLayOutHelper::add(Drawable2D* obj)
{
    m_parent->addChild(obj);
    m_objList.push_back(obj);
}

void HorizonalLayOutHelper::doLayout()
{
    Drawable2D * lastObj = nullptr;
    vec2 pos = m_origin;
    for(auto obj : m_objList)
    {
        
        if(lastObj)
        {
            pos.x += lastObj->getContentSize().x + m_padding;
        }
        obj->setPos2D(pos);
        lastObj = obj;
    }
}

HorizonalLayOut::HorizonalLayOut()
{
}

HorizonalLayOut::HorizonalLayOut(Node* parent)
{
    m_padding = 5.f;
    parent->addChild(this);
}

void HorizonalLayOut::add(Drawable2D* obj)
{
    m_padding = 5.f;
    addChild(obj);
}

void HorizonalLayOut::doLayout()
{
    Drawable2D * lastObj = nullptr;
    vec2 pos = vec2(0, 0);
    vec2 tmpContentSize;
    for(auto obj : m_children)
    {
        if(lastObj)
        {
            pos.x += lastObj->getContentSize().x + m_padding;
        }
        obj->setPos2D(pos);
        tmpContentSize.y = std::max(((Drawable2D*)obj)->getContentSize().y , tmpContentSize.y);
        lastObj = (Drawable2D * )obj;
    }
    tmpContentSize.x = pos.x + lastObj->getContentSize().x;
    setContentSize(tmpContentSize);
}

void HorizonalLayOut::logicUpdate(float dt)
{
    doLayout();
}



VerticalLayOut::VerticalLayOut()
{
    m_padding = 2.f;
}

VerticalLayOut::VerticalLayOut(Node* parent)
{
    m_padding = 2.f;
    parent->addChild(this);
}

void VerticalLayOut::add(Drawable2D* obj)
{
    addChild(obj);
}

void VerticalLayOut::doLayout()
{
    Drawable2D * lastObj = nullptr;
    vec2 pos = vec2(0, 0);
    vec2 tmpContentSize;
    for(auto obj : m_children)
    {
        if(lastObj)
        {
            pos.y += lastObj->getContentSize().y + m_padding;
        }
        obj->setPos2D(pos);
        tmpContentSize.x = std::max(((Drawable2D*)obj)->getContentSize().x , tmpContentSize.x);
        lastObj = (Drawable2D * )obj;
    }
    tmpContentSize.y = pos.y + lastObj->getContentSize().y;
    setContentSize(tmpContentSize);
}

void VerticalLayOut::logicUpdate(float dt)
{
    doLayout();
}

} // namespace tzw
