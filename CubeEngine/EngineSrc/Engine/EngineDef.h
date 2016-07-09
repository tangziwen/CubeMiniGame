#ifndef TZW_ENGINECONFIG_H
#define TZW_ENGINECONFIG_H

#define TZW_SINGLETON_DECL(className) public: static className * m_instance;\
    static className * shared();
#define TZW_SINGLETON_IMPL(className) className * className::m_instance = nullptr;\
className * className::shared(){if(!m_instance){m_instance = new className();}return m_instance;}


namespace tzw {
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
    static const char * versionStr;
    static int focusPiority;
};
} // namespace tzw

#endif // TZW_ENGINECONFIG_H
