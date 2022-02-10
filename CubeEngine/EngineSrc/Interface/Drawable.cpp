#include "Drawable.h"
#include "Scene/SceneMgr.h"

namespace tzw {

Drawable::Drawable()
    :m_camera(nullptr),m_color(vec4(1, 1, 1, 1))
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
	auto currCam = g_GetCurrScene()->defaultCamera();
    info.m_projectMatrix = currCam->projection();
    info.m_viewMatrix = currCam->getViewMatrix();
    info.m_worldMatrix = getTransform();
}

void Drawable::setUpCommand(RenderCommand & command)
{
	setUpTransFormation(command.m_transInfo);
}

void Drawable::setColor(vec4 newColor)
{
	m_color = newColor;
	if (!getMaterial()) return;
	getMaterial()->setVar("TU_color", newColor);
}

void Drawable::setColor(vec3 newColorV3)
{
    setColor(vec4(newColorV3, 1.0f));
}

vec4 Drawable::getColor()
{
	return m_color;
}
} // namespace tzw

