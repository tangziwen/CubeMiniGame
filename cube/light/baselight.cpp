#include "baselight.h"

BaseLight::BaseLight()
{
    this->intensity = 1;
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
