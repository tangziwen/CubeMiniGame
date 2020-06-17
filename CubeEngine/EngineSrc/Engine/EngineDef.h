#ifndef TZW_ENGINECONFIG_H
#define TZW_ENGINECONFIG_H
#include "Base/Singleton.h"
#define SAFE_DELETE(PTR) delete PTR; PTR = NULL

#define HIGH_FOUR_BIT(a) ((a & 0xf0) >> 4)
#define LOW_FOUR_BIT(a) (a & 0x0f)

#define SET_HIGH_FOUR_BIT(a, val) a = ((a & 0x0f) | (val << 4))
#define SET_LOW_FOUR_BIT(a, val) a = ((a & 0xf0) | (val))
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
