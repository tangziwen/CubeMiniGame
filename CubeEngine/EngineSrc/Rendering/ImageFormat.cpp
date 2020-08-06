#include "ImageFormat.h"

size_t tzw::ImageFormatGetSize(ImageFormat format)
{
    switch(format){
    case ImageFormat::R8:
        return 1;
    case ImageFormat::R8G8:
        return 2;
    case ImageFormat::R8G8B8:
        return 3;
    case ImageFormat::R8G8B8A8:
        return 4;
    }
    return 1;
}
