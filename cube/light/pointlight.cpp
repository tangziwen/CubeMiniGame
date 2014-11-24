#include "pointlight.h"
#include "QDebug"
PointLight::PointLight()
{
    this->color = QVector3D(1,1,1);
    this->pos = QVector3D (0,2,0);
    this->range = 1;
}

QVector3D PointLight::getColor()
{
    return this->color;
}

void PointLight::setColor(QVector3D new_color)
{
    this->color = new_color;
}

QVector3D PointLight::getPos()
{
    return this->pos;
}

void PointLight::setPos(QVector3D new_pos)
{
    this->pos = new_pos;
}

void PointLight::apply(ShaderProgram *shader,int index)
{
    initializeOpenGLFunctions();
    char tmp[100];
    sprintf(tmp,"pointLight[%d].pos",index);
    int pos_ptr=glGetUniformLocation(shader->getShaderId(),tmp);
    sprintf(tmp,"pointLight[%d].color",index);
    int color_ptr=glGetUniformLocation(shader->getShaderId(),tmp);
    sprintf(tmp,"pointLight[%d].range",index);
    int attenuation_ptr = glGetUniformLocation(shader->getShaderId(),tmp);
    sprintf(tmp,"pointLight[%d].intensity",index);
    int intensity_ptr = glGetUniformLocation(shader->getShaderId(),tmp);
    glUniform3f(pos_ptr,pos.x(),pos.y(),pos.z());
    glUniform3f(color_ptr,color.x(),color.y(),color.z());
    glUniform1f(attenuation_ptr,range);
    glUniform1f(intensity_ptr,1);
}

float PointLight::getRange()
{
    return this->range;
}

void PointLight::setRange(float new_attenuation)
{
    this->range = new_attenuation;
}
