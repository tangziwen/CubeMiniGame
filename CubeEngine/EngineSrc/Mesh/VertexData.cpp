#include "VertexData.h"

namespace tzw {

VertexData::VertexData(vec3 pos)
    :m_pos(pos),m_texCoord(vec2()),m_normal(vec3())
{

}

VertexData::VertexData(vec3 pos, vec2 texCoord)
    :m_pos(pos),m_texCoord(texCoord),m_normal(vec3())
{

}

VertexData::VertexData(vec3 pos, vec3 normal,vec2 texCoord)
    :m_pos(pos),m_normal(normal),m_texCoord(texCoord)
{

}
} // namespace tzw

