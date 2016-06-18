#ifndef TZW_GUIPALETTE_H
#define TZW_GUIPALETTE_H

#include "../Math/vec4.h"
namespace tzw {

struct GUIStyle
{
    vec4 backGroundColor;
    vec4 buttonFrameColor;
    vec4 buttonFrameColorHightLight;
    GUIStyle();
};

} // namespace tzw

#endif // TZW_GUIPALETTE_H
