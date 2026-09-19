#include "ImageFormat.h"

size_t tzw::ImageFormatGetSize(ImageFormat format)
{
    switch(format)
    {
    case ImageFormat::R8_UNorm: return 1;
    case ImageFormat::RG8_UNorm:
    case ImageFormat::D16_UNorm: return 2;
    case ImageFormat::RGB8_UNorm:
    case ImageFormat::D16_UNorm_S8_UInt: return 3;
    case ImageFormat::RGBA8_UNorm:
    case ImageFormat::RGBA8_SNorm:
    case ImageFormat::D24_UNorm_S8_UInt:
    case ImageFormat::D32_Float: return 4;
    case ImageFormat::RGB16_UNorm:
    case ImageFormat::RGB16_Float: return 6;
    case ImageFormat::RGBA16_UNorm:
    case ImageFormat::RGBA16_Float:
    case ImageFormat::D32_Float_S8_UInt: return 8;
    case ImageFormat::Surface: return 0;
    }
    return 0;
}

bool tzw::ImageFormatIsDepth(ImageFormat format)
{
    return format == ImageFormat::D16_UNorm || format == ImageFormat::D32_Float
        || ImageFormatHasStencil(format);
}

bool tzw::ImageFormatHasStencil(ImageFormat format)
{
    return format == ImageFormat::D16_UNorm_S8_UInt || format == ImageFormat::D24_UNorm_S8_UInt
        || format == ImageFormat::D32_Float_S8_UInt;
}
