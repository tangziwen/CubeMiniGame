#ifndef TZW_MATERIAL_H
#define TZW_MATERIAL_H
#include "../../Texture/Texture.h"

namespace tzw {

class Material
{
public:
    Material();

    Texture *diffuseMap() const;
    void setDiffuseMap(Texture *diffuseMap);

    Texture *specularMap() const;
    void setSpecularMap(Texture *specularMap);

    Texture *ambientMap() const;
    void setAmbientMap(Texture *ambientMap);

    Texture *emissiveMap() const;
    void setEmissiveMap(Texture *emissiveMap);

    Texture *heightMap() const;
    void setHeightMap(Texture *heightMap);

    Texture *normalMap() const;
    void setNormalMap(Texture *normalMap);

    Texture *shininessMap() const;
    void setShininessMap(Texture *shininessMap);

    Texture *opacityMap() const;
    void setOpacityMap(Texture *opacityMap);

    Texture *displacementMap() const;
    void setDisplacementMap(Texture *displacementMap);

    Texture *lightMap() const;
    void setLightMap(Texture *lightMap);

    Texture *reflectionMap() const;
    void setReflectionMap(Texture *reflectionMap);
protected:
    Texture * m_diffuseMap;
    Texture * m_specularMap;
    Texture * m_ambientMap;
    Texture * m_emissiveMap;
    Texture * m_heightMap;
    Texture * m_normalMap;
    Texture * m_shininessMap;
    Texture * m_opacityMap;
    Texture * m_displacementMap;
    Texture * m_lightMap;
    Texture * m_reflectionMap;
};
} // namespace tzw

#endif // TZW_MATERIAL_H
