#ifndef TZW_VERTEXDATA_H
#define TZW_VERTEXDATA_H

#include "../Math/vec3.h"
#include "../Math/vec2.h"
#include "../Math/vec4.h"
namespace tzw {
struct MatBlendInfo
{
	char matIndex1;
	char matIndex2;
	char matIndex3;
	vec3 matBlendFactor;
};
class VertexData
{
public:
    VertexData();
    VertexData(vec3 pos, vec2 texCoord);
	VertexData(vec3 pos, vec2 texCoord, vec4 color);
    VertexData(vec3 pos, vec3 normal,vec2 texCoord);
    VertexData(vec3 pos);
	VertexData(vec3 pos, vec4 color);
	VertexData(vec3 pos, vec3 normal);
    vec3 m_pos;
    vec3 m_normal;
    vec2 m_texCoord;
    vec4 m_color;
    vec3 m_barycentric;
	char m_matIndex[3];
	vec3 m_matBlendFactor;
	vec3 m_tangent;
};

class VertexDataLite
{
public:
	VertexDataLite();
	vec4 m_data1;
	vec4 m_data2;
};
} // namespace tzw

#endif // TZW_VERTEXDATA_H
