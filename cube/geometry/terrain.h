#ifndef TERRAIN_H
#define TERRAIN_H
#include <QOpenGLFunctions>
#include "mesh.h"
class Terrain :protected QOpenGLFunctions
{
public:
    Terrain(const char * fileName);
    TMesh *mesh() const;
    void setMesh(TMesh *mesh);
protected:
    TMesh * m_mesh;
};

#endif // TERRAIN_H
