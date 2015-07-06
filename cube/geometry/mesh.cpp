#include "mesh.h"
#include "texture/texturepool.h"

TMesh::TMesh()
{
    for(int i =0;i<3;i++)
    {
        vbo[i] =0;
    }
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

static  void CaculateTangent(GLushort * indices,int indices_count,VertexData  * vertices,int vertices_count  )
{

    for (unsigned int i = 0 ; i < indices_count; i += 3) {
        auto v0 = vertices[indices[i]];
        auto v1 = vertices[indices[i+1]];
        auto v2 = vertices[indices[i+2]];

        auto Edge1 = v1.position - v0.position;
        auto Edge2 = v2.position - v0.position;

        float DeltaU1 = v1.texCoord.x ()- v0.texCoord.x ();
        float DeltaV1 = v1.texCoord.y() - v0.texCoord.y();
        float DeltaU2 = v2.texCoord.x() - v0.texCoord.x();
        float DeltaV2 = v2.texCoord.y() - v0.texCoord.y();

        float f = 1.0f / (DeltaU1 * DeltaV2 - DeltaU2 * DeltaV1);

        QVector3D Tangent, Bitangent;

        Tangent.setX (f * (DeltaV2 * Edge1.x() - DeltaV1 * Edge2.x()));
        Tangent.setY (f * (DeltaV2 * Edge1.y() - DeltaV1 * Edge2.y()));
        Tangent.setZ (f * (DeltaV2 * Edge1.z() - DeltaV1 * Edge2.z()));

        Bitangent.setX (f * (-DeltaU2 * Edge1.x() - DeltaU1 * Edge2.x()));
        Bitangent.setY (f * (-DeltaU2 * Edge1.y() - DeltaU1 * Edge2.y()));
        Bitangent.setZ (f * (-DeltaU2 * Edge1.z() - DeltaU1 * Edge2.z()));

        vertices[indices[i]].tangent += Tangent;
        vertices[indices[i+1]].tangent += Tangent;
        vertices[indices[i+2]].tangent += Tangent;
    }

    for (unsigned int i = 0 ; i < vertices_count ; i++) {
        vertices[i].tangent.normalize ();
    }
}


void TMesh::finishWithoutNormal()
{
    initializeGLFunctions();
    if(!vbo[0])
    {
        glGenBuffers(2, vbo);
    }


    CaculateNormalLine(&this->indices[0],this->indices.size(),&this->vertices[0],this->vertices.size());

    CaculateTangent(&this->indices[0],this->indices.size(),&this->vertices[0],this->vertices.size());

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VertexData), &vertices[0], GL_STATIC_DRAW);

    // Transfer index data to VBO 1
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), &indices[0], GL_STATIC_DRAW);
    if(!this->material->getDiffuse()->texture)
    {
        this->material->getDiffuse()->texture = TexturePool::getInstance()->createOrGetTexture("default");
    }
    for(int i =0;i<vertices.size ();i++)
    {
        m_aabb.update (&vertices[i].position,1);
    }
    draw_command.Init(this->material,vbo[0],vbo[1],indices.size());
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


    if(!this->material->getDiffuse()->texture)
    {
        this->material->getDiffuse()->texture = TexturePool::getInstance()->createOrGetTexture("default");
    }
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
AABB TMesh::aabb() const
{
    return m_aabb;
}

void TMesh::setAabb(const AABB &aabb)
{
    m_aabb = aabb;
}

VertexData *TMesh::getVertex(int index)
{
    return &vertices[index];
}


