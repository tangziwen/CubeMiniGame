#ifndef TZW_CMC_VERTEX_H
#define TZW_CMC_VERTEX_H
#include <QVector2D>
#include <QVector3D>
#include "external/rapidjson/document.h"
#include "external/rapidjson/prettywriter.h"
#include "external/rapidjson/filewritestream.h"
#include "external/rapidjson/stringbuffer.h"
namespace tzw {
#define MAX_BONES 4
class CMC_Vertex
{
public:
    CMC_Vertex();
    ~CMC_Vertex();
    QVector3D pos() const;
    void setPos(const QVector3D &pos);

    QVector2D UV() const;
    void setUV(const QVector2D &UV);

    QVector3D normal() const;
    void setNormal(const QVector3D &normal);

    QVector3D tangent() const;
    void setTangent(const QVector3D &tangent);

    void write( rapidjson::PrettyWriter<rapidjson::StringBuffer> & writer);

    void loadFromTZW(const rapidjson::Value &value);
    void addBoneData(int boneIndex, float boneWeight);

    int m_boneIds[MAX_BONES];
    float m_boneWeights[MAX_BONES];
private:
    QVector3D m_pos;
    QVector2D m_UV;
    QVector3D m_normal;
    QVector3D m_tangent;
};

} // namespace tzw

#endif // TZW_CMC_VERTEX_H
