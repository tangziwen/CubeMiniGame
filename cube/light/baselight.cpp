#include "baselight.h"

BaseLight::BaseLight()
{
    this->intensity = 1;
    m_shadowFBO = new ShadowMapFBO();
    m_shadowFBO->Init (1024,768);
}

QVector3D BaseLight::getColor()
{
    return this->color;
}

void BaseLight::setColor(QVector3D new_color)
{
    this->color = new_color;
}

float BaseLight::getIntensity()
{
    return this->intensity;
}

void BaseLight::setIntensity(float new_intensity)
{
    this->intensity = new_intensity;
}


void BaseLight::apply(ShaderProgram *program, int index)
{

}
ShadowMapFBO *BaseLight::shadowFBO() const
{
    return m_shadowFBO;
}

void BaseLight::setShadowFBO(ShadowMapFBO *shadowFBO)
{
    m_shadowFBO = shadowFBO;
}

