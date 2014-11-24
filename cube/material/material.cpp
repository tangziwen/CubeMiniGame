#include "material.h"

Material::Material()
{
    this->m_normalMap = NULL;
}

MaterialChannel *Material::getDiffuse()
{
    return & this->diffuse;
}

MaterialChannel *Material::getAmbient()
{
    return & this->ambient;
}

MaterialChannel *Material::getSpecular()
{
    return & this->specular;
}
Texture *Material::normalMap() const
{
    return m_normalMap;
}

void Material::setNormalMap(Texture *normalMap)
{
    m_normalMap = normalMap;
}

