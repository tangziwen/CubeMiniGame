#ifndef MATERIALCHANNEL_H
#define MATERIALCHANNEL_H
#include <qvector3d.h>
#include "texture/texture.h"
class MaterialChannel
{
public:
    MaterialChannel();
    QVector3D color;
    Texture * texture;
};

#endif // MATERIALCHANNEL_H
