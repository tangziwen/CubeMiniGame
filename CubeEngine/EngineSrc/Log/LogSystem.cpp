#include "LogSystem.h"

#include <stdarg.h>
#include <stdio.h>
namespace tzw {

LogSystem * LogSystem::m_instance = nullptr;

LogSystem *LogSystem::shared()
{
    if(!m_instance)
    {
        m_instance = new LogSystem;
    }
    return m_instance;
}

void LogSystem::log(const char *format,...)
{
    va_list ap;
    va_start(ap,format);
    vfprintf(stdout,format,ap);
    va_end(ap);
    fflush(stdout);
    return ;
}

void LogSystem::log(std::string str)
{
    log(str.c_str());
}

void LogSystem::logStr(std::string str)
{
    log(str);
}


LogSystem::LogSystem()
{

}

} // namespace tzw

