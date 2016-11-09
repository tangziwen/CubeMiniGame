#ifndef TZW_LOGSYSTEM_H
#define TZW_LOGSYSTEM_H
#include <string>
namespace tzw {

class LogSystem
{
public:
    static LogSystem * shared();
    void log(const char * format,...);
    void log(std::string str);
    void logStr(std::string str);
private:
    static LogSystem * m_instance;
    LogSystem();
};

} // namespace tzw

#define T_LOG (LogSystem::shared()->log)
#endif // TZW_LOGSYSTEM_H
