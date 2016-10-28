#include "Tmisc.h"
#include <windows.h>
#include <stdarg.h>
namespace tzw {

std::wstring Tmisc::StringToWString(const std::string &str)
{
    int len;
    int slength = (int)str.length() + 1;
    len = MultiByteToWideChar(65001, 0, str.c_str(), slength, 0, 0);
    wchar_t* buf = new wchar_t[len];
    MultiByteToWideChar(65001, 0, str.c_str(), slength, buf, len);
    std::wstring r(buf);
    delete[] buf;
    return r;
}

std::string Tmisc::WstringToString(const std::wstring &wstr)
{
    auto nLen = wstr.length() + 1;
    char * buf = new char[nLen];
    auto multiLen = WideCharToMultiByte(65001,0,(LPCWSTR)wstr.c_str(),(int)nLen,(LPSTR)buf,0,NULL,NULL);
    int nResult = WideCharToMultiByte(65001,0,(LPCWSTR)wstr.c_str(),(int)nLen,(LPSTR)buf,(int)multiLen,NULL,NULL);
    if (nResult == 0)
    {
        return std::string(" ");
    }
    return std::string(buf);
}

std::string Tmisc::StrFormat(const char *format,...)
{
    va_list argptr;
    static char tmp[128];
    va_start(argptr, format);
    vsprintf(tmp, format, argptr);
    va_end(argptr);
    return std::string(tmp);
}

static clock_t _theClock;
void Tmisc::DurationBegin()
{
    _theClock = clock();
}

clock_t Tmisc::DurationEnd()
{
    return clock() - _theClock;
}

} // namespace tzw
