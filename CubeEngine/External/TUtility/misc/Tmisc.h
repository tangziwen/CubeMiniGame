#ifndef TZW_TMISC_H
#define TZW_TMISC_H

#include <string>
#define T_INVALID(p) !p

namespace tzw {

class Tmisc
{
public:
	static std::wstring StringToWString(const std::string &str);
	static std::string WstringToString(const std::wstring &wstr);
	static std::string StrFormat(const char * format,...);
};

} // namespace tzw

#endif // TZW_TMISC_H
