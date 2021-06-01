#pragma once

#include <stdint.h>
namespace tzw {
namespace RenderFlag
{
enum class BufferTarget{
    IndexBuffer,
    VertexBuffer
};
enum class IndicesType{
	Lines,
    Triangles,
    TriangleStrip,
    Patches,
};

enum class TextureType
{
    Texture2D,
    TextureCubeMap,
};

enum class RenderFunction
{
    DepthTest,
    AlphaBlend
};

enum class DepthTestMethod
{
    Less,
    LessOrEqual
};

enum class WarpAddress
{
    Repeat,
    Clamp,
    ClampToEdge,
};

enum class BlendingEquation
{
    Add,
};

enum class BlendingFactor
{
    One,
    Zero,
    SrcAlpha,
    OneMinusSrcAlpha,
    ConstantAlpha
};


enum class RenderStageType
{
    COMMON,
    SHADOW,
};
enum class RenderStage : uint32_t
{
	COMMON = 1UL <<1,
	TRANSPARENT = 1UL <<2,
	AFTER_DEPTH_CLEAR = 1UL <<3,
	GUI= 1UL <<4,
	SHADOW = 1UL <<5,
	All = -1
};

enum class BufferStorageType
{
	STATIC_DRAW,
	DYNAMIC_DRAW,
};

enum class CullMode{
    Back,
    Front,
};
}
} // namespace tzw

