#include "Drawable.h"

namespace tzw {

Drawable::Drawable()
    :m_camera(nullptr)
{
    m_material = nullptr;
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

Node::NodeType Drawable::getNodeType()
{
    return NodeType::Drawable;
}

void Drawable::reCache()
{
    Node::reCache();
}

Material *Drawable::getMaterial() const
{
    return m_material;
}

void Drawable::setMaterial(Material *technique)
{
    m_material = technique;
}

void Drawable::setUpTransFormation(TransformationInfo &info)
{
    info.m_projectMatrix = camera()->projection();
    info.m_viewMatrix = camera()->getViewMatrix();
    info.m_worldMatrix = getTransform();
}

void Drawable::setUpCommand(RenderCommand & command)
{
	setUpTransFormation(command.m_transInfo);
}

void Drawable::setColor(vec4 newColor)
{
	if (!m_material) return;
	m_material->setVar("color", newColor);
}

} // namespace tzw

