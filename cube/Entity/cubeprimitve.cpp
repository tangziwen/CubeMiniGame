#include "cubeprimitve.h"
#include "texture/texturepool.h"
TMesh * static_mesh = NULL;
CubePrimitve::CubePrimitve(const char * textureFile)
{
    Texture * tex = TexturePool::getInstance()->createOrGetTexture (textureFile);
    Material * mat = new Material();
    mat->getDiffuse ()->texture = tex;
    TMesh * mesh = new TMesh();
    if(!static_mesh)
    {
        mesh->setMaterial (mat);
        //front
        mesh->pushVertex (VertexData(QVector3D(-0.5,0.5,0.5),QVector2D(0,0)));//0
        mesh->pushVertex (VertexData(QVector3D(-0.5,-0.5,0.5),QVector2D(0,1)));//1
        mesh->pushVertex (VertexData(QVector3D(0.5,-0.5,0.5),QVector2D(1,1)));//2
        mesh->pushVertex (VertexData(QVector3D(0.5,0.5,0.5),QVector2D(1,0)));//3
        //back
        mesh->pushVertex (VertexData(QVector3D(0.5,0.5,-0.5),QVector2D(0,0)));//4
        mesh->pushVertex (VertexData(QVector3D(0.5,-0.5,-0.5),QVector2D(0,1)));//5
        mesh->pushVertex (VertexData(QVector3D(-0.5,-0.5,-0.5),QVector2D(1,1)));//6
        mesh->pushVertex (VertexData(QVector3D(-0.5,0.5,-0.5),QVector2D(1,0)));//7
        //top
        mesh->pushVertex (VertexData(QVector3D(-0.5,0.5,-0.5),QVector2D(0,1)));//8
        mesh->pushVertex (VertexData(QVector3D(-0.5,0.5,0.5),QVector2D(0,0)));//9
        mesh->pushVertex (VertexData(QVector3D(0.5,0.5,0.5),QVector2D(1,0)));//10
        mesh->pushVertex (VertexData(QVector3D(0.5,0.5,-0.5),QVector2D(1,1)));//11
        //bottom
        mesh->pushVertex (VertexData(QVector3D(-0.5,-0.5,-0.5),QVector2D(0,1)));//12
        mesh->pushVertex (VertexData(QVector3D(-0.5,-0.5,0.5),QVector2D(0,0)));//13
        mesh->pushVertex (VertexData(QVector3D(0.5,-0.5,0.5),QVector2D(1,0)));//14
        mesh->pushVertex (VertexData(QVector3D(0.5,-0.5,-0.5),QVector2D(1,1)));//15
        //left
        mesh->pushVertex (VertexData(QVector3D(-0.5,0.5,-0.5),QVector2D(0,0)));//16
        mesh->pushVertex (VertexData(QVector3D(-0.5,-0.5,-0.5),QVector2D(0,1)));//17
        mesh->pushVertex (VertexData(QVector3D(-0.5,-0.5,0.5),QVector2D(1,1)));//18
        mesh->pushVertex (VertexData(QVector3D(-0.5,0.5,0.5),QVector2D(1,0)));//19

        //right
        mesh->pushVertex (VertexData(QVector3D(0.5,0.5,-0.5),QVector2D(1,0)));//20
        mesh->pushVertex (VertexData(QVector3D(0.5,-0.5,-0.5),QVector2D(1,1)));//21
        mesh->pushVertex (VertexData(QVector3D(0.5,-0.5,0.5),QVector2D(0,1)));//22
        mesh->pushVertex (VertexData(QVector3D(0.5,0.5,0.5),QVector2D(0,0)));//23
        GLushort indices[] = {0,1,3,1,2,3,  20,21,22,20,22,23,   4,5,6,4,6,7,  16,17,18,16,18,19,    13,12,15,14,13,15,    8,9,10,8,10,11 };
        mesh->pushIndex (indices,sizeof(indices)/sizeof(GLushort));
        mesh->finishWithoutNormal ();
        static_mesh = mesh;
    }else
    {
        *mesh = *static_mesh;
        mesh->setMaterial (mat);
    }
    addMesh (mesh);
}

CubePrimitve::~CubePrimitve()
{

}

