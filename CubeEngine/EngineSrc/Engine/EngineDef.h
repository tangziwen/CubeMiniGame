#ifndef TZW_ENGINECONFIG_H
#define TZW_ENGINECONFIG_H
#include "Base/Singleton.h"
namespace tzw
{

typedef int integer;
typedef unsigned int integer_u;
typedef unsigned short short_u;
class EngineDef
{
public:
    enum class MouseButton
    {
        LeftButton,
        RightButton,
    };
    static int maxPiority;
	static int debugPanelPiority;
    static const char * versionStr;
    static int focusPiority;
};
} // namespace tzw

#endif // TZW_ENGINECONFIG_H
