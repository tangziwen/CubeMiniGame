#ifndef MATERIAL_H
#define MATERIAL_H
#include "materialchannel.h"
#include "texture/texture.h"
class Material
{
public:
    Material();
    MaterialChannel * getDiffuse();
    MaterialChannel * getAmbient();
    MaterialChannel * getSpecular();
    Texture *normalMap() const;
    void setNormalMap(Texture *normalMap);
private:
    MaterialChannel diffuse;
    MaterialChannel ambient;
    MaterialChannel specular;
    Texture * m_normalMap;
};

#endif // MATERIAL_H
