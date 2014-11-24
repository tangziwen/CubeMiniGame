#include "spotlight.h"
#include "utility.h"
#include "math.h"
SpotLight::SpotLight()
{
    this->angle = utility::Ang2Radius(60);
    this->outterAngle = utility::Ang2Radius(65);
    this->direction = QVector3D(-1,0,0);
    this->pos = QVector3D(1,0,0);
}

void SpotLight::setAngle(float new_angle)
{
    this->angle = new_angle;
}

float SpotLight::getAngle()
{
    return this->angle;
}

void SpotLight::setOutterAngle(float new_angle)
{
    this->outterAngle = new_angle;
}

float SpotLight::getOutterAngle()
{
    return this->outterAngle;
}

void SpotLight::apply(ShaderProgram *shader,int index)
{
    initializeOpenGLFunctions();
    char tmp[100];
    sprintf(tmp,"spotLight[%d].pos",index);
    int pos_ptr=glGetUniformLocation(shader->getShaderId(),tmp);
    sprintf(tmp,"spotLight[%d].color",index);
    int color_ptr=glGetUniformLocation(shader->getShaderId(),tmp);
    sprintf(tmp,"spotLight[%d].range",index);
    int attenuation_ptr = glGetUniformLocation(shader->getShaderId(),tmp);
    sprintf(tmp,"spotLight[%d].intensity",index);
    int intensity_ptr = glGetUniformLocation(shader->getShaderId(),tmp);
    sprintf(tmp,"spotLight[%d].ang",index);
    int angle_ptr = glGetUniformLocation(shader->getShaderId(),tmp);
    sprintf(tmp,"spotLight[%d].outterAng",index);
    int outterAngle_ptr = glGetUniformLocation(shader->getShaderId(),tmp);
    sprintf(tmp,"spotLight[%d].direction",index);
    int direction_ptr = glGetUniformLocation(shader->getShaderId(),tmp);
    glUniform3f(pos_ptr,pos.x(),pos.y(),pos.z());
    glUniform3f(color_ptr,color.x(),color.y(),color.z());
    glUniform1f(attenuation_ptr,range);
    glUniform1f(intensity_ptr,intensity);
    glUniform1f(angle_ptr,cos(angle));
    glUniform1f(outterAngle_ptr,cos(this->outterAngle));
    glUniform3f(direction_ptr,this->direction.x(),this->direction.y(),this->direction.z());
}

void SpotLight::setDirection(QVector3D new_direction)
{
    this->direction = new_direction;
}

QVector3D SpotLight::getDirection()
{
    return this->direction ;
}
