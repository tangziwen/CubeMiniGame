#ifndef TZW_ENGINECONFIG_H
#define TZW_ENGINECONFIG_H
#include "Base/Singleton.h"
#include <string>
#include <unordered_map>

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


//injection manually binding reflection

#define T_PROP_REFLECT_REG(PROP) m_propsLink[#PROP] = &m_##PROP;//regReflect##PROP();

#define T_PROP_REFLECT_REG_BEGIN(CLASS_NAME)\
	void init_reg_reflect_##CLASS_NAME(CLASS_NAME *obj)\
	{

#define T_PROP_REFLECT_REG_END }\
protected:\
int m_dullObj = [this](){init_reg_reflect();return 0;}();

#define T_PROP_REFLECT_DO(CLASSNAME) init_reg_reflect_##CLASSNAME(this);

#define T_PROP_REFLECT_REG_DECLEAR() void init_reg_reflect(){



class TObjectReflect
{
public:
	template<class T>
	void setPropByName(std::string name, T val)
	{
		T * iter = (T *)m_propsLink[name];
		(*iter) = val;
	}
	template<class T>
	void addPropByName(std::string name, T val)
	{
		T * iter = (T *)m_propsLink[name];
		(*iter) += val;
	}
	template<class T>
	T getPropByName(std::string name)
	{
		T * iter = (T *)m_propsLink[name];
		return (*iter);
	}

protected:
	std::unordered_map<std::string, void *> m_propsLink;
};


} // namespace tzw

#endif // TZW_ENGINECONFIG_H
