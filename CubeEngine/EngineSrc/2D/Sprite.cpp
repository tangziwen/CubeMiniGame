#include "Sprite.h"
#include "../Rendering/Renderer.h"
#include "../Scene/SceneMgr.h"
#include "EngineSrc/3D/Effect/EffectMgr.h"
namespace tzw {

Sprite::Sprite()
    :m_isUseTexture(true),m_texture(nullptr)
{
    setCamera(g_GetCurrScene()->defaultGUICamera());
}

/**
 * @brief Sprite::create 创建精灵的工厂方法
 * @param texturePath 制定的纹理路径
 * @return 被创建的精灵
 */
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

}

void Sprite::initWithTexture(Texture *texture)
{
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

void Sprite::submitDrawCmd()
{
    RenderCommand command(m_mesh,m_material,RenderCommand::RenderType::GUI);
    setUpTransFormation(command.m_transInfo);
    command.setZorder(m_globalPiority);
    Renderer::shared()->addRenderCommand(command);
}
/**
 * @brief Sprite::setRenderRect 设置精灵的渲染区域，分别指定了精灵在屏幕上的大小，以及其渲染的纹理的区域大小
 * @param size 在场景中的实际尺寸
 * @param lb 渲染纹理的区域的左下角
 * @param rt 渲染纹理区域的右上角
 */
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


/**
 * @brief Sprite::setContentSize 设置精灵的当前尺寸
 * @param contentSize 精灵的新尺寸
 */
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
        m_material->initFromEffect("Sprite");
        m_material->setTex("SpriteTexture", m_texture);
    }else
    {
        m_material->initFromEffect("SpriteColor");
        m_material = Material::createFromEffect("SpriteColor");
    }
    m_material->setVar("color",getUniformColor());
}

} // namespace tzw

