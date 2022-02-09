#include "Sprite.h"
#include "../Scene/SceneMgr.h"
#include "EngineSrc/3D/Effect/EffectMgr.h"
#include "Technique/MaterialPool.h"
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
    setRenderRect(m_contentSize, vec2(0,0), vec2(1,1), m_uniformColor);
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
    setRenderRect(m_contentSize, vec2(0,0), vec2(1,1), m_uniformColor);
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
    Drawable2D::setContentSize(contentSize);
    setRenderRect(m_contentSize, vec2(0,0), vec2(1,1), m_uniformColor);
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

void Sprite::setRenderRect(vec2 size, vec2 lb, vec2 rt, vec4 color)
{
    auto width = size.x;
    auto height = size.y;
    m_mesh->clearVertices();
    auto vertex_0 = VertexData(vec3(0,0,-1),vec2(lb.x,lb.y));
    vertex_0.m_color = color;
    m_mesh->addVertex(vertex_0);// left bottom
    auto  vertex_1 = VertexData(vec3(width,0,-1),vec2(rt.x,lb.y));
    vertex_1.m_color = color;
    m_mesh->addVertex(vertex_1);// right bottom
    auto  vertex_2 = VertexData(vec3(width,height,-1),vec2(rt.x,rt.y));
    vertex_2.m_color = color;
    m_mesh->addVertex(vertex_2); // right top
    auto vertex_3 = VertexData(vec3(0,height,-1),vec2(lb.x,rt.y));
    vertex_3.m_color = color;
    m_mesh->addVertex(vertex_3); // left top
    m_mesh->finish(true);
}

void Sprite::setRenderRect( vec4 v1, vec4 v2,vec4 v3, vec4 v4, vec4 color)
{
    m_mesh->clearVertices();
    auto vertex_0 = VertexData(vec3(v1.x,v1.y,-1),vec2(v1.z,v1.w));
    m_mesh->addVertex(vertex_0);// left bottom
    auto vertex_1 = VertexData(vec3(v2.x,v2.y,-1),vec2(v2.z,v2.w));
    m_mesh->addVertex(vertex_1);// right bottom
    auto vertex_2 = VertexData(vec3(v3.x,v3.y,-1),vec2(v3.z,v3.w));
    m_mesh->addVertex(vertex_2); // right top
    auto vertex_3 = VertexData(vec3(v4.x,v4.y,-1),vec2(v4.z,v4.w));
    m_mesh->addVertex(vertex_3); // left top
    m_mesh->finish(true);


    //update content size
    AABB aabb;
    aabb.update(v1.toVec3());
    aabb.update(v2.toVec3());
    aabb.update(v3.toVec3());
    aabb.update(v4.toVec3());
    //m_contentSize.x = aabb.max().x - aabb.min().x;
    //m_contentSize.y = aabb.max().y - aabb.min().y;
    Drawable2D::setContentSize(vec2(aabb.max().x - aabb.min().x, aabb.max().y - aabb.min().y));
}


void Sprite::setContentSize(const vec2 &contentSize)
{
    //m_contentSize = contentSize;
    Drawable2D::setContentSize(contentSize);
    setRenderRect(m_contentSize, vec2(0,0), vec2(1,1), m_uniformColor);
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
        m_material->setVar("color",getUniformColor());

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
} // namespace tzw

