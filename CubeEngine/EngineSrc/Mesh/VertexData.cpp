#include "VertexData.h"

namespace tzw {

VertexData::VertexData(vec3 pos)
    :m_pos(pos),m_texCoord(vec2()),m_normal(vec3()), m_color(vec4(1.0, 1.0, 1.0, 1.0))
{

}

VertexData::VertexData()
:m_pos(vec3()),m_texCoord(vec2()),m_normal(vec3()), m_color(vec4(1.0, 1.0, 1.0, 1.0))
{

}

VertexData::VertexData(vec3 pos, vec2 texCoord)
    :m_pos(pos),m_texCoord(texCoord),m_normal(vec3()), m_color(vec4(1.0, 1.0, 1.0, 1.0))
{

}

VertexData::VertexData(vec3 pos, vec3 normal,vec2 texCoord)
    :m_pos(pos),m_normal(normal),m_texCoord(texCoord), m_color(vec4(1.0, 1.0, 1.0, 1.0))
{

}

VertexData::VertexData(vec3 pos, vec4 color)
	:m_pos(pos),m_normal(vec3()),m_texCoord(vec2()), m_color(color)
{

}

} // namespace tzw

