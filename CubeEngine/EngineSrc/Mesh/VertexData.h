#ifndef TZW_VERTEXDATA_H
#define TZW_VERTEXDATA_H

#include "../Math/vec3.h"
#include "../Math/vec2.h"
namespace tzw {

class VertexData
{
public:
    VertexData();
    VertexData(vec3 pos, vec2 texCoord);
    VertexData(vec3 pos, vec3 normal,vec2 texCoord);
    VertexData(vec3 pos);
    vec3 m_pos;
    vec3 m_normal;
    vec2 m_texCoord;
    vec3 m_color;
};

} // namespace tzw

#endif // TZW_VERTEXDATA_H
