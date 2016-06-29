#include "Material.h"

namespace tzw {

Material::Material()
{

}

Texture *Material::diffuseMap() const
{
    return m_diffuseMap;
}

void Material::setDiffuseMap(Texture *diffuseMap)
{
    m_diffuseMap = diffuseMap;
}

Texture *Material::specularMap() const
{
    return m_specularMap;
}

void Material::setSpecularMap(Texture *specularMap)
{
    m_specularMap = specularMap;
}

Texture *Material::ambientMap() const
{
    return m_ambientMap;
}

void Material::setAmbientMap(Texture *ambientMap)
{
    m_ambientMap = ambientMap;
}

Texture *Material::emissiveMap() const
{
    return m_emissiveMap;
}

void Material::setEmissiveMap(Texture *emissiveMap)
{
    m_emissiveMap = emissiveMap;
}

Texture *Material::heightMap() const
{
    return m_heightMap;
}

void Material::setHeightMap(Texture *heightMap)
{
    m_heightMap = heightMap;
}

Texture *Material::normalMap() const
{
    return m_normalMap;
}

void Material::setNormalMap(Texture *normalMap)
{
    m_normalMap = normalMap;
}

Texture *Material::shininessMap() const
{
    return m_shininessMap;
}

void Material::setShininessMap(Texture *shininessMap)
{
    m_shininessMap = shininessMap;
}

Texture *Material::opacityMap() const
{
    return m_opacityMap;
}

void Material::setOpacityMap(Texture *opacityMap)
{
    m_opacityMap = opacityMap;
}

Texture *Material::displacementMap() const
{
    return m_displacementMap;
}

void Material::setDisplacementMap(Texture *displacementMap)
{
    m_displacementMap = displacementMap;
}

Texture *Material::lightMap() const
{
    return m_lightMap;
}

void Material::setLightMap(Texture *lightMap)
{
    m_lightMap = lightMap;
}

Texture *Material::reflectionMap() const
{
    return m_reflectionMap;
}

void Material::setReflectionMap(Texture *reflectionMap)
{
    m_reflectionMap = reflectionMap;
}

} // namespace tzw
