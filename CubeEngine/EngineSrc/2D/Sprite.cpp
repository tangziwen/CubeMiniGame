#include "Sprite.h"
#include "../Scene/SceneMgr.h"
#include "EngineSrc/3D/Effect/EffectMgr.h"
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

void Sprite::initWithTexture(std::string texturePath)
{
	m_material = new Material();
    m_mesh = new tzw::Mesh();
    m_mesh->addIndex(0);
    m_mesh->addIndex(1);
    m_mesh->addIndex(2);
    m_mesh->addIndex(0);
    m_mesh->addIndex(2);
    m_mesh->addIndex(3);
    m_texture = TextureMgr::shared()->getByPath(texturePath);
    m_contentSize = m_texture->getSize();
    setRenderRect(m_contentSize);
    setUpTechnique();
    m_material->setRenderStage(RenderFlag::RenderStage::GUI);

}

void Sprite::initWithTexture(Texture *texture)
{
	m_material = new Material();
    m_mesh = new tzw::Mesh();
    m_mesh->addIndex(0);
    m_mesh->addIndex(1);
    m_mesh->addIndex(2);
    m_mesh->addIndex(0);
    m_mesh->addIndex(2);
    m_mesh->addIndex(3);
    m_texture = texture;
    m_contentSize = m_texture->getSize();
    setRenderRect(m_contentSize);
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
    setUniformColor(color);
    m_contentSize = contentSize;
    setRenderRect(m_contentSize);
}

void Sprite::submitDrawCmd(RenderFlag::RenderStage requirementType, RenderQueue * queues, int requirementArg)
{
	//getContentSize();
	//getWorldPos2D();
    RenderCommand command(m_mesh,m_material,this, RenderFlag::RenderStage::GUI);
    setUpTransFormation(command.m_transInfo);
    command.setZorder(m_globalPiority);
    queues->addRenderCommand(command, requirementArg);
}

void Sprite::setRenderRect(vec2 size, vec2 lb, vec2 rt)
{
    auto width = size.x;
    auto height = size.y;
    m_mesh->clearVertices();
    m_mesh->addVertex(tzw::VertexData(vec3(0,0,-1),vec2(lb.x,lb.y)));// left bottom
    m_mesh->addVertex(tzw::VertexData(vec3(width,0,-1),vec2(rt.x,lb.y)));// right bottom
    m_mesh->addVertex(tzw::VertexData(vec3(width,height,-1),vec2(rt.x,rt.y))); // right top
    m_mesh->addVertex(tzw::VertexData(vec3(0,height,-1),vec2(lb.x,rt.y))); // left top
    m_mesh->finish(true);
}

void Sprite::setRenderRect( vec4 v1, vec4 v2,vec4 v3, vec4 v4)
{
    m_mesh->clearVertices();
    m_mesh->addVertex(tzw::VertexData(vec3(v1.x,v1.y,-1),vec2(v1.z,v1.w)));// left bottom
    m_mesh->addVertex(tzw::VertexData(vec3(v2.x,v2.y,-1),vec2(v2.z,v2.w)));// right bottom
    m_mesh->addVertex(tzw::VertexData(vec3(v3.x,v3.y,-1),vec2(v3.z,v3.w))); // right top
    m_mesh->addVertex(tzw::VertexData(vec3(v4.x,v4.y,-1),vec2(v4.z,v4.w))); // left top
    m_mesh->finish(true);


    //update content size
    AABB aabb;
    aabb.update(v1.toVec3());
    aabb.update(v2.toVec3());
    aabb.update(v3.toVec3());
    aabb.update(v4.toVec3());
    m_contentSize.x = aabb.max().x - aabb.min().x;
    m_contentSize.y = aabb.max().y - aabb.min().y;
}


void Sprite::setContentSize(const vec2 &contentSize)
{
    m_contentSize = contentSize;
    setRenderRect(m_contentSize);
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
    if(m_texture)
    {
        m_material->loadFromTemplate("Sprite");
        m_material->setTex("SpriteTexture", m_texture);
    }else
    {
        m_material->loadFromTemplate("SpriteColor");
        m_material = Material::createFromTemplate("SpriteColor");
    }
    m_material->setVar("color",getUniformColor());
}

void Sprite::setUpTransFormation(TransformationInfo& info)
{
	auto currCam = g_GetCurrScene()->defaultGUICamera();
    info.m_projectMatrix = currCam->projection();
    info.m_viewMatrix = currCam->getViewMatrix();
    info.m_worldMatrix = getTransform();
}
} // namespace tzw

