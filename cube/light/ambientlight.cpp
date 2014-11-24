#include "ambientlight.h"

AmbientLight::AmbientLight()
{
    this->color = QVector3D(1,1,1);
    this->intensity = 0.3;
}

void AmbientLight::setColor(QVector3D new_color)
{
    this->color = new_color;
}

QVector3D AmbientLight::getColor()
{
    return this->color;
}

void AmbientLight::apply(ShaderProgram *shader)
{
    shader->setUniform3Float("ambient.color",color.x(),color.y(),color.z());
    shader->setUniformFloat("ambient.intensity",intensity);
}
