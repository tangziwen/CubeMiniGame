#include "Sprite.h"
#include "../Scene/SceneMgr.h"
#include "EngineSrc/3D/Effect/EffectMgr.h"
#include "Technique/MaterialPool.h"
#include "../Event/EventMgr.h"
#include "Engine/Engine.h"
namespace tzw {

Sprite::Sprite()
    :m_isUseTexture(true),m_texture(nullptr)
{
    setCamera(g_GetCurrScene()->defaultGUICamera());
}


Sprite *Sprite::create(std::string texturePath)
{
    auto newObj = new Sprite();
    newObj->initWithTexture(texturePath);
    return newObj;
}

Sprite *Sprite::createWithColor(vec4 color,vec2 contentSize)
{
    auto newObj = new Sprite();
    newObj->initWithColor(color,contentSize);
    return newObj;
}

Sprite::~Sprite()
{
    removeFromParent();
}

void Sprite::initWithTexture(std::string texturePath)
{
    //MaterialPool::shared()->getMaterialByName(getSpriteManggledName())
	//m_material = new Material();
    m_mesh = new tzw::Mesh();
    m_mesh->addIndex(0);
    m_mesh->addIndex(1);
    m_mesh->addIndex(2);
    m_mesh->addIndex(0);
    m_mesh->addIndex(2);
    m_mesh->addIndex(3);
    m_texture = TextureMgr::shared()->getByPath(texturePath);
    Drawable2D::setContentSize(m_texture->getSize());
    setRenderRect(m_contentSize, m_lb, m_rt, m_color);
    setUpTechnique();
    m_material->setRenderStage(RenderFlag::RenderStage::GUI);

}

void Sprite::initWithTexture(Texture *texture)
{
	//m_material = new Material();
    m_mesh = new tzw::Mesh();
    m_mesh->addIndex(0);
    m_mesh->addIndex(1);
    m_mesh->addIndex(2);
    m_mesh->addIndex(0);
    m_mesh->addIndex(2);
    m_mesh->addIndex(3);
    m_texture = texture;
    m_contentSize = m_texture->getSize();
    Drawable2D::setContentSize(m_texture->getSize());
    setRenderRect(m_contentSize, m_lb, m_rt, m_color);
    setUpTechnique();
}

void Sprite::initWithColor(vec4 color,vec2 contentSize)
{
	m_material = new Material();
    m_mesh = new tzw::Mesh();
    setUpTechnique();
    m_mesh->addIndex(0);
    m_mesh->addIndex(1);
    m_mesh->addIndex(2);
    m_mesh->addIndex(0);
    m_mesh->addIndex(2);
    m_mesh->addIndex(3);
    setColor(color);
    Drawable2D::setContentSize(contentSize);
    setRenderRect(m_contentSize, m_lb, m_rt, m_color);
}

void Sprite::submitDrawCmd(RenderFlag::RenderStage requirementType, RenderQueue * queues, int requirementArg)
{

    if(m_isRenderRectDirty)
    {

        setRenderRect(m_contentSize, m_lb, m_rt, m_color);
        m_isRenderRectDirty = false;
    }
    RenderCommand command(m_mesh,m_material,this, RenderFlag::RenderStage::GUI);
    setUpTransFormation(command.m_transInfo);
    command.setZorder(m_globalPiority);
    queues->addRenderCommand(command, requirementArg);
}


void Sprite::setRenderRect(vec2 size, vec2 lb, vec2 rt, vec4 color)
{
    auto width = size.x;
    auto height = size.y;
    m_mesh->clearVertices();
    auto vertex_0 = VertexData(vec3(0,0,-1),vec2(lb.x,lb.y));
    vertex_0.m_color = color;
	vertex_0.m_overlayColor = m_overLayColor;
    m_mesh->addVertex(vertex_0);// left bottom
    auto  vertex_1 = VertexData(vec3(width,0,-1),vec2(rt.x,lb.y));
    vertex_1.m_color = color;
	vertex_1.m_overlayColor = m_overLayColor;
    m_mesh->addVertex(vertex_1);// right bottom
    auto  vertex_2 = VertexData(vec3(width,height,-1),vec2(rt.x,rt.y));
    vertex_2.m_color = color;
	vertex_2.m_overlayColor = m_overLayColor;
    m_mesh->addVertex(vertex_2); // right top
    auto vertex_3 = VertexData(vec3(0,height,-1),vec2(lb.x,rt.y));
    vertex_3.m_color = color;
	vertex_3.m_overlayColor = m_overLayColor;
    m_mesh->addVertex(vertex_3); // left top
    if(isFirstTimeUpdateRenderRect)
    {
        m_mesh->finish(true);
    }
    else
    {
        m_mesh->submitVBO();
    }
    isFirstTimeUpdateRenderRect = false;
}

void Sprite::setContentSize(const vec2 &contentSize)
{
    //m_contentSize = contentSize;
    Drawable2D::setContentSize(contentSize);
    m_isRenderRectDirty = true;
    //setRenderRect(m_contentSize, vec2(0,0), vec2(1,1), m_color);
}

Texture *Sprite::texture() const
{
    return m_texture;
}

void Sprite::setTexture(Texture *texture)
{
    m_texture = texture;
    m_material->setTex("SpriteTexture", texture);
}

void Sprite::setTexture(std::string texturePath)
{
    setTexture(TextureMgr::shared()->getByPath(texturePath));
}

bool Sprite::isUseTexture() const
{
    return m_isUseTexture;
}

void Sprite::setIsUseTexture(bool isUseTexture)
{
    m_isUseTexture = isUseTexture;
}

void Sprite::setUpTechnique()
{
    m_material = MaterialPool::shared()->getMaterialByName(getSpriteManggledName());
    if (!m_material)
    {
        m_material = new Material();
        if(m_texture)
        {
            m_material->loadFromTemplate("Sprite");
            m_material->setTex("SpriteTexture", m_texture);
        }else
        {
            m_material->loadFromTemplate("SpriteColor");
            m_material = Material::createFromTemplate("SpriteColor");
        }
        m_material->setVar("color",m_color);

        MaterialPool::shared()->addMaterial(getSpriteManggledName(), m_material);
    }
}

void Sprite::setUpTransFormation(TransformationInfo& info)
{
	auto currCam = g_GetCurrScene()->defaultGUICamera();
    info.m_projectMatrix = currCam->projection();
    info.m_viewMatrix = currCam->getViewMatrix();
    info.m_worldMatrix = getTransform();
}

void Sprite::setColor(vec4 newColor)
{
    m_isRenderRectDirty = true;
    m_color = newColor;
}

void Sprite::setOverLayColor(vec4 newColor)
{
	m_isRenderRectDirty = true;
	m_overLayColor = newColor;
}

bool Sprite::onMouseRelease(int button, vec2 pos)
{
    if(m_isTouched)
    {
        if(isInTheRect(pos))
        {
            if(m_onBtnClicked)
            {
                m_onBtnClicked(this);
            }
            m_isTouched = false;
            return true;
        }
    }
    return false;
}

bool Sprite::onMousePress(int button, vec2 pos)
{
    if(isInTheRect(pos))
    {
        m_isTouched = true;
    }
    return false;
}

bool Sprite::onMouseMove(vec2 pos)
{
    return false;
}

void Sprite::setTouchEnable(bool isEnable)
{
    bool changed = m_isTouchEnable != isEnable;
    if(changed)
    {
        if(isEnable)
        {
            EventMgr::shared()->addNodePiorityListener(this,this);
        }
        else
        {
            EventMgr::shared()->removeNodeEventListener(this);
        }
    }
}


std::string Sprite::getSpriteManggledName()
{
    std::string name = "Sprite:";
    if (m_texture)
    {
        name += m_texture->getFilePath();
    }
    else
    {
        name += "withColor";
    }
    return name;
}
bool Sprite::isInTheRect(vec2 touchPos)
{
    vec2 winSize = Engine::shared()->winSize();
    touchPos /= winSize;
    touchPos.y = 1.0 - touchPos.y;//flip y
    touchPos = touchPos* 2.0f - vec2(1.0, 1.0);//NDC
    
    auto mvp = g_GetCurrScene()->defaultGUICamera()->getViewProjectionMatrix() * getTransform();
    Matrix44 inverseTransform = mvp.inverted();
    vec4 touchInLocalSpace = inverseTransform.transofrmVec4(vec4(touchPos.x, touchPos.y, 0.0, 1.0));
    touchInLocalSpace.x /= touchInLocalSpace.w;
    touchInLocalSpace.y /= touchInLocalSpace.w;

    if (touchInLocalSpace.x >=0 && touchInLocalSpace.x<= m_contentSize.x  && touchInLocalSpace.y >=0 && touchInLocalSpace.y<= m_contentSize.y)
    {
        return true;
    }else
    {
        return false;
    }
}
void Sprite::setOnBtnClicked(const std::function<void(Sprite*)>& onBtnClicked)
{
    m_onBtnClicked = onBtnClicked;
}
} // namespace tzw

