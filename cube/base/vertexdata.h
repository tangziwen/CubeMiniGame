#ifndef VERTEXDATA_H
#define VERTEXDATA_H

#include <QVector2D>
#include <QVector3D>
#include <QVector4D>
struct VertexData
{
    VertexData();
    VertexData(QVector3D pos);
    VertexData(QVector3D pos,QVector2D texCoord);
    VertexData(QVector3D pos,QVector2D texCoord,QVector3D normal);

    QVector3D position;
    QVector2D texCoord;
    QVector3D normalLine;
    int boneId[4];
    float boneWeight[4];
    QVector3D tangent;
    void addBoneData(int id, float weight);
};

#endif // VERTEXDATA_H
