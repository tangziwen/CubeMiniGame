#include "GUIStyleMgr.h"

namespace tzw {


GUIStyleMgr::GUIStyleMgr()
{
    m_defaultPalette = new GUIStyle();
}

GUIStyle *GUIStyleMgr::defaultPalette() const
{
    return m_defaultPalette;
}

void GUIStyleMgr::setDefaultPalette(GUIStyle *defaultPalette)
{
    m_defaultPalette = defaultPalette;
}

} // namespace tzw
