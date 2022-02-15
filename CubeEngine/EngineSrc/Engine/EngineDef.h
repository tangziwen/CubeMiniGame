#ifndef TZW_ENGINECONFIG_H
#define TZW_ENGINECONFIG_H
#include "Base/Singleton.h"
#define SAFE_DELETE(PTR) delete PTR; PTR = NULL

#define HIGH_FOUR_BIT(a) ((a & 0xf0) >> 4)
#define LOW_FOUR_BIT(a) (a & 0x0f)

#define SET_HIGH_FOUR_BIT(a, val) a = ((a & 0x0f) | (val << 4))
#define SET_LOW_FOUR_BIT(a, val) a = ((a & 0xf0) | (val))
#define TZW_USE_SIMD 1
namespace tzw
{

typedef int integer;
typedef unsigned int integer_u;
typedef unsigned short short_u;

enum class RenderDeviceType{
    Vulkan_Device,
    OpenGl_Device,
};
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
    static bool isUseVulkan;
};
//property
#define TZW_PROPERTY(PROP_TYPE, PROP, DEFAULT_VALUE)\
public:\
PROP_TYPE get##PROP(){return m_##PROP;};\
void set##PROP(PROP_TYPE obj){m_##PROP = obj;};\
protected:\
PROP_TYPE m_##PROP {DEFAULT_VALUE};

} // namespace tzw

#endif // TZW_ENGINECONFIG_H
