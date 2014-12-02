#include "mesh.h"
#include "texture/texturepool.h"

TMesh::TMesh()
{

}

void TMesh::pushVertex(VertexData vertex)
{
    this->vertices.push_back(vertex);
}

void TMesh::pushVertex(VertexData *vertex, int size)
{
    for(int i =0;i<size;i++)
    {
        this->pushVertex(vertex[i]);
    }
}

void TMesh::pushIndex(GLushort index)
{
    this->indices.push_back(index);
}

void TMesh::pushIndex(GLushort *array, int size)
{
    for(int i=0;i<size;i++)
    {
        this->pushIndex(array[i]);
    }
}

static  void CaculateNormalLine(GLushort * indices,int indices_count,VertexData  * vertices,int vertices_count  )
{

    for (unsigned int i = 0 ; i < indices_count ; i += 3) {
        unsigned int Index0 = indices[i];
        unsigned int Index1 = indices[i + 1];
        unsigned int Index2 = indices[i + 2];
        QVector3D v1 = vertices[Index1].position - vertices[Index0].position;
        QVector3D v2 = vertices[Index2].position - vertices[Index0].position;
        QVector3D Normal = QVector3D::crossProduct(v1,v2);
        Normal.normalize();
        vertices[Index0].normalLine += Normal;
        vertices[Index1].normalLine += Normal;
        vertices[Index2].normalLine += Normal;
    }
    for (unsigned int i = 0 ; i < vertices_count ; i++) {
        vertices[i].normalLine.normalize();
    }
}


void TMesh::finishWithoutNormal()
{
    initializeGLFunctions();
    glGenBuffers(2, vbo);
    CaculateNormalLine(&this->indices[0],this->indices.size(),&this->vertices[0],this->vertices.size());

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VertexData), &vertices[0], GL_STATIC_DRAW);

    // Transfer index data to VBO 1
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), &indices[0], GL_STATIC_DRAW);
    if(!this->material->getDiffuse()->texture)
    {
        this->material->getDiffuse()->texture = TexturePool::getInstance()->createTexture("default");
    }
    draw_command.Init(this->material,vbo[0],vbo[1],indices.size());
}

void TMesh::setBones(std::vector<BoneData> bones)
{
    this->m_bones = bones;
    for(int i =0 ;i<bones.size();i++)
    {
        for(int j =0; j<4;j++)
        {
            this->vertices[i].boneId[j] = bones[i].IDs[j];
            this->vertices[i].boneWeight[j] = bones[i].weights[j];
        }

    }
}

void TMesh::finish()
{
    initializeGLFunctions();

    glGenBuffers(3, vbo);

    //vertices
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VertexData), &vertices[0], GL_STATIC_DRAW);

    //indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), &indices[0], GL_STATIC_DRAW);

    //bone
    glBindBuffer(GL_ARRAY_BUFFER,vbo[2]);
    glBufferData(GL_ARRAY_BUFFER,sizeof(BoneData)*m_bones.size(),&m_bones[0],GL_STATIC_DRAW);

    if(!this->material->getDiffuse()->texture)
    {
        this->material->getDiffuse()->texture = TexturePool::getInstance()->createTexture("default");
    }
    draw_command.m_bones = this->m_bones;
    draw_command.Init(this->material,vbo[0],vbo[1],vbo[2],indices.size());
}

void TMesh::checkIndexValid(int index)
{

}

void TMesh::draw()
{
    draw_command.Draw();
}

int TMesh::getVerticesNumber()
{
    return this->vertices.size();
}

VertexData *TMesh::at(int index)
{
    return &this->vertices[index] ;
}


GLuint TMesh::getVerticesVbo()
{
    return this->vbo[0];
}

GLuint TMesh::getIndicesVbo()
{
    return vbo[1];
}

MeshDrawComand *TMesh::getCommand()
{
    return &(this->draw_command);
}
Material *TMesh::getMaterial() const
{
    return material;
}

void TMesh::setMaterial(Material *value)
{
    material = value;
}

