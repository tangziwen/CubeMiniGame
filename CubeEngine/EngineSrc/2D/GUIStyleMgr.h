#ifndef TZW_GUIPALETTEMGR_H
#define TZW_GUIPALETTEMGR_H
#include "GUIStyle.h"

#include "../Engine/EngineDef.h"

namespace tzw {

class GUIStyleMgr
{
public:
    TZW_SINGLETON_DECL(GUIStyleMgr)
    GUIStyle *defaultPalette() const;
    void setDefaultPalette(GUIStyle *defaultPalette);

private:
    GUIStyleMgr();
    GUIStyle * m_defaultPalette;
};

} // namespace tzw

#endif // TZW_GUIPALETTEMGR_H
