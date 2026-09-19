#pragma once
#include <cstddef>

namespace tzw
{
enum class ImageFormat
{
    R8_UNorm,
    RG8_UNorm,
    RGB8_UNorm,
    RGBA8_UNorm,
    RGBA16_UNorm,
    D16_UNorm,
    D24_UNorm_S8_UInt,
    D16_UNorm_S8_UInt,
    RGBA8_SNorm,
    RGBA16_Float,
    Surface,
    RGB16_UNorm,
    RGB16_Float,
    D32_Float,
    D32_Float_S8_UInt,
};
size_t ImageFormatGetSize(ImageFormat format);
bool ImageFormatIsDepth(ImageFormat format);
bool ImageFormatHasStencil(ImageFormat format);
}
