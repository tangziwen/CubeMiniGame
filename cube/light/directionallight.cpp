#include "directionallight.h"

DirectionalLight::DirectionalLight()
{
    this->color = QVector3D(1,1,1);
    this->direction = QVector3D(0,0,1);
    this->intensity = 0;
}

void DirectionalLight::setDirection(QVector3D new_direction)
{
    this->direction = new_direction;
}

QVector3D DirectionalLight::getDirection()
{
    return this->direction;
}

void DirectionalLight::setColor(QVector3D new_color)
{
    this->color = new_color;
}

QVector3D DirectionalLight::getColor()
{
    return this->color;
}

void DirectionalLight::apply(ShaderProgram *shader)
{
    shader->setUniform3Float("directionLight.direction",direction.x(),direction.y(),direction.z());
    shader->setUniform3Float("directionLight.color",color.x(),color.y(),color.z());
    shader->setUniformFloat("directionLight.intensity",intensity);
}
