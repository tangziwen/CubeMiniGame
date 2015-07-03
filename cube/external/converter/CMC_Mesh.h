#ifndef TZW_CMC_MESH_H
#define TZW_CMC_MESH_H

#include "CMC_Vertex.h"
#include <vector>
#include "external/rapidjson/document.h"
#include "external/rapidjson/prettywriter.h"
#include "external/rapidjson/filewritestream.h"
#include "external/rapidjson/stringbuffer.h"
namespace tzw {

class CMC_Mesh
{
public:
    CMC_Mesh();
    ~CMC_Mesh();
    CMC_Vertex * getVertex(int index);
    void pushVertex(CMC_Vertex vertex);
    void pushIndex(unsigned short index);
    void write(rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer);
    int materialIndex() const;
    void setMaterialIndex(int materialIndex);
    void loadFromTZW(const rapidjson::Value &value);
    std::vector<CMC_Vertex> m_vertices;
    std::vector<unsigned short>m_indices;
private:

    int m_materialIndex;
};

} // namespace tzw

#endif // TZW_CMC_MESH_H
