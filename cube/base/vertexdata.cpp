#include "vertexdata.h"
#include <assert.h>
#define MAX_BONES 4
VertexData::VertexData()
{
    this->position=QVector3D(0,0,0);
    this->texCoord=QVector2D(0,0);
    this->normalLine=QVector3D(0,0,0);
    for(int i =0 ;i<4;i++) {boneId[i]=0;boneWeight[i]=0.0;}
}

VertexData::VertexData(QVector3D pos)
{
    this->position = pos;
    for(int i =0 ;i<4;i++) {boneId[i]=0;boneWeight[i]=0.25;}
}

VertexData::VertexData(QVector3D pos, QVector2D texCoord)
{
    this->position=pos;
    this->texCoord=texCoord;
    this->normalLine=QVector3D(0,0,0);
    for(int i =0 ;i<4;i++) {boneId[i]=0;boneWeight[i]=0.25;}
}

VertexData::VertexData(QVector3D pos, QVector2D texCoord, QVector3D normal)
{
    this->position=pos;
    this->texCoord=texCoord;
    this->normalLine=normal;
    for(int i =0 ;i<4;i++) {boneId[i]=0;boneWeight[i]=0.25;}
}

void VertexData::addBoneData(int id, float weight)
{
    for (int i = 0 ; i < MAX_BONES ; i++) {
        if (boneWeight[i] == 0.0) {
            boneId[i]     = id;
            boneWeight[i] = weight;
            return;
        }
    }
    // should never get here - more bones than we have space for
    assert(0);
}
