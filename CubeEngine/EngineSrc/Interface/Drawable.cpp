#include "Drawable.h"

namespace tzw {

Drawable::Drawable()
    :m_uniformColor(vec4(1.0,1.0,1.0,1.0)),m_technique(nullptr),m_camera(nullptr)
{
}
/**
 * @brief Drawable::camera 获取当前对象的相机
 * @return 当前对象的相机
 */
Camera *Drawable::camera() const
{
    return m_camera;
}
/**
 * @brief Drawable::setCamera 设置当前对象的相机
 * @param camera
 */
void Drawable::setCamera(Camera *camera)
{
    m_camera = camera;
}

/**
 * @brief Drawable::technique 获取当前对象的technique对象指针
 * @return techinique对象指针
 */
Technique *Drawable::technique() const
{
    return m_technique;
}

/**
 * @brief Drawable::setTechnique 为当前Drawable对象设置新的technique对象指针
 * @param technique 新的technique对象指针
 */
void Drawable::setTechnique(Technique *technique)
{
    m_technique = technique;
}

Node::NodeType Drawable::getNodeType()
{
    return NodeType::Drawable;
}
vec4 Drawable::getUniformColor() const
{
    return m_uniformColor;
}

void Drawable::setUniformColor(const vec4 &uniformColor)
{
    m_uniformColor = uniformColor;
    if(m_technique)
    {
        m_technique->setVar("TU_color",m_uniformColor);
    }
}

void Drawable::setUniformColor(const vec3 &color)
{
    setUniformColor(vec4(color.x,color.y,color.z,1.0f));
}

void Drawable::setAlpha(float alpha)
{
    m_uniformColor.w = alpha;
    if(m_technique)
    {
        m_technique->setVar("TU_color",m_uniformColor);
    }
}

void Drawable::reCache()
{
    Node::reCache();
}




} // namespace tzw

