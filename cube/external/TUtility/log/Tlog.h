#ifndef TZW_TLOG_H
#define TZW_TLOG_H
#include <string>
#include "../string/Tstring.h"
#include <stdio.h>
namespace tzw {

class TlogTitleEnd
{
};

class TlogTitleBegin
{
};

class TlogEndLine
{

};

enum class TlogPolicy
{
    CONSOLE,
    CONSOLE_FILE,
    FILE
};

enum class TlogLevel
{
    LOG_VERBOSE,
    LOG_WARNING,
    LOG_ERROR,
    TLOG_CRITICAL_ERROR,
};
typedef void (* writeFunc)(const char *);
class TlogSystem
{
public:
    static TlogSystem * get();
    TlogPolicy getPolicy() const;
    void setPolicy(const TlogPolicy &policy);
    FILE *getFileHandle() const;
    void setFileHandle(FILE *fileHandle);
    TlogLevel getLevel() const;
    void setLevel(const TlogLevel &level);
    void setWriteFunc(writeFunc func);
    writeFunc getWriteFunc();
private:
    TlogSystem();
    static TlogSystem * system;
    TlogPolicy m_policy;
    FILE * m_fileHandle;
    TlogLevel m_level;
    writeFunc m_writeFunc;
};

class Tlog
{
public:
    Tlog(TlogLevel level =TlogLevel::LOG_VERBOSE);
    ~Tlog();
     Tlog &operator <<(const char * str) ;
     Tlog &operator <<(const float f);
     Tlog &operator <<(const int i) ;
     Tlog &operator <<(const char c) ;
     Tlog &operator <<(const double d) ;
     Tlog &operator <<(const long l) ;
     Tlog &operator <<(const bool b) ;
     Tlog &operator <<(const std::string & str) ;
     Tlog &operator <<(const Tstring & str) ;
     Tlog &operator <<(const TlogTitleEnd & el) ;
     Tlog &operator <<(const TlogTitleBegin & begin) ;
private:
    TlogLevel m_level;
    std::string m_buffer;
};

//global variable for log  begin end
static TlogTitleBegin TlogBegin;
static TlogTitleEnd TlogEnd;
} // namespace tzw

#define T_Log_CRASH_NOW(A) (Tlog()<<TlogBegin<<"crash at "<<__FILE__<<" lines "<<__LINE__<<", in function: "<<__func__<<TlogEL<<A<<TlogEnd,abort())

#define T_Log_EXIT_NOW(A) (Tlog()<<TlogBegin<<"crash at "<<__FILE__<<" lines "<<__LINE__<<", in function: "<<__func__<<TlogEL<<A<<TlogEnd,exit(0))

#define T_Log_AT(A) (Tlog()<<TlogBegin<<"log at "<<__FILE__<<" lines "<<__LINE__<<", in function: "<<__func__<<TlogEL<<A<<TlogEnd)
#endif // TZW_TLOG_H

