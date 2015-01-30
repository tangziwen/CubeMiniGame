#include "terrain.h"
#include <external/SOIL/SOIL.h>
#include <QDebug>
#define MAP_SCALE 0.1
#define MAP_HEIGHT 1
#include "material/materialpool.h"
Terrain::Terrain(const char *fileName)
{
    initializeOpenGLFunctions ();
    m_mesh = new TMesh();
    unsigned char * result;
    int width =0;
    int height =0;
    int channels = 0;
    result= SOIL_load_image (fileName,&width,&height,&channels,SOIL_LOAD_RGB);
    for(int i =0;i<width;i++)
    {
        for(int j=0;j<height;j++)
        {
            VertexData v;
            v.position = QVector3D(j*MAP_SCALE- height/2*MAP_SCALE,
                                  result[(i*width+j)*3]*1.0/255*MAP_HEIGHT -0.5*MAP_HEIGHT,
                    i*MAP_SCALE - width/2*MAP_SCALE);
            v.texCoord = QVector2D(j*1.0/width,i*1.0/height);
            m_mesh->pushVertex (v);
        }
    }

    for(int i =0;i<width-1;i++)
    {
        for(int j = 0;j<height-1;j++)
        {
            m_mesh->pushIndex (i*width+j);
            m_mesh->pushIndex ((i+1)*width+j);
            m_mesh->pushIndex (i*width+j+1);

            m_mesh->pushIndex (i*width+j+1);
            m_mesh->pushIndex ((i+1)*width+j);
            m_mesh->pushIndex ((i+1)*width+j+1);
        }
    }
    m_mesh->setMaterial (MaterialPool::getInstance ()->createOrGetMaterial ("default"));
    m_mesh->finishWithoutNormal ();
}
TMesh *Terrain::mesh() const
{
    return m_mesh;
}

void Terrain::setMesh(TMesh *mesh)
{
    m_mesh = mesh;
}

