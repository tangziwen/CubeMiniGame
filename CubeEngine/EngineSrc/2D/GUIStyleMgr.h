#ifndef TZW_GUIPALETTEMGR_H
#define TZW_GUIPALETTEMGR_H
#include "GUIStyle.h"

#include "../Engine/EngineDef.h"

namespace tzw {

class GUIStyleMgr : public Singleton<GUIStyleMgr>
{
public:
    GUIStyle *defaultPalette() const;
    void setDefaultPalette(GUIStyle *defaultPalette);
	GUIStyleMgr();
private:
    
    GUIStyle * m_defaultPalette;
};

} // namespace tzw

#endif // TZW_GUIPALETTEMGR_H
