#pragma once

#include <cstdint>
#include <vector>

namespace tzw
{
enum class VertexAttributeFormat
{
    Float2,
    Float3,
    Float4,
    UNorm8x4,
    UInt8x3,
};

struct VertexAttribute
{
    VertexAttributeFormat format = VertexAttributeFormat::Float2;
    uint32_t offset = 0;
};

struct VertexLayout
{
    uint32_t stride = 0;
    std::vector<VertexAttribute> attributes;
};
}
