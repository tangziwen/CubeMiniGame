#ifndef TZW_MATERIAL_STATE_H
#define TZW_MATERIAL_STATE_H

namespace tzw
{

enum MaterialFlag
{
	MaterialFlag_isCullFace = 1 << 1,
	MaterialFlag_isInstanced = 1 << 2,
	MaterialFlag_isBlend = 1 << 3,
	MaterialFlag_isDepthWrite = 1 << 4,
	MaterialFlag_isDepthTest = 1 << 5,
	Shader_option_End = 1 << 31,
};

enum class PrimitiveTopology
{
	TriangleList,
	LineList,
};

enum class RasterFillMode
{
	Fill,
	Wireframe,
};

}

#endif // TZW_MATERIAL_STATE_H
