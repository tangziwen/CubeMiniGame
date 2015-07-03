#include "CMC_Vertex.h"


namespace tzw {
enum AttributeOffset
{
    X_offset,
    Y_offset,
    Z_offset,
    U_offset,
    V_offset,
    NX_offset,
    NY_offset,
    NZ_offset,
    TX_offset,
    TY_offset,
    TZ_offset,
};
CMC_Vertex::CMC_Vertex()
{

    for(int i =0; i<MAX_BONES; i++)
    {
        m_boneWeights[i] = 0;
    }
}

CMC_Vertex::~CMC_Vertex()
{

}
QVector3D CMC_Vertex::pos() const
{
    return m_pos;
}

void CMC_Vertex::setPos(const QVector3D &pos)
{
    m_pos = pos;
}
QVector2D CMC_Vertex::UV() const
{
    return m_UV;
}

void CMC_Vertex::setUV(const QVector2D &UV)
{
    m_UV = UV;
}
QVector3D CMC_Vertex::normal() const
{
    return m_normal;
}

void CMC_Vertex::setNormal(const QVector3D &normal)
{
    m_normal = normal;
}
QVector3D CMC_Vertex::tangent() const
{
    return m_tangent;
}

void CMC_Vertex::setTangent(const QVector3D &tangent)
{
    m_tangent = tangent;
}

void CMC_Vertex::write(rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer)
{
    writer.StartArray ();
    //pos
    writer.Double (m_pos.x ());
    writer.Double (m_pos.y ());
    writer.Double (m_pos.z ());
    //UV
    writer.Double (m_UV.x ());
    writer.Double (m_UV.y ());
    //normal
    writer.Double (m_normal.x ());
    writer.Double (m_normal.y ());
    writer.Double (m_normal.z ());
    //tangent -- for normal mapping
    writer.Double (m_tangent.x ());
    writer.Double (m_tangent.y ());
    writer.Double (m_tangent.z ());
    writer.EndArray ();
}

void CMC_Vertex::loadFromTZW(const rapidjson::Value &value)
{
    //pos
    m_pos = QVector3D(value[X_offset].GetDouble (),value[Y_offset].GetDouble (),value[Z_offset].GetDouble ());
    //uv
    m_UV = QVector2D(value[U_offset].GetDouble (),value[V_offset].GetDouble ());
    //normal
    m_normal = QVector3D(value[NX_offset].GetDouble (),value[NY_offset].GetDouble (),value[NZ_offset].GetDouble ());
    //tangent
    m_tangent = QVector3D(value[TX_offset].GetDouble (),value[TY_offset].GetDouble (),value[TZ_offset].GetDouble ());
}

void CMC_Vertex::addBoneData(int boneIndex, float boneWeight)
{
    for (int i = 0 ; i < MAX_BONES ; i++) {
        if (m_boneWeights[i] == 0.0) {
            m_boneIds[i]     = boneIndex;
            m_boneWeights[i] = boneWeight;
            return;
        }
    }
    // should never get here - more bones than we have space for
    assert(0);
}





} // namespace tzw

