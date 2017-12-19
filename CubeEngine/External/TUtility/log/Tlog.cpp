#include "Tlog.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#ifdef  USE_QT
#include <QDebug>
#endif

namespace tzw {

TlogTitleBegin TlogBegin;
TlogTitleEnd TlogEnd;
TlogSystem * TlogSystem::system = NULL;

static char tmpStr[512];

Tlog * TlogSystem::preTlog = NULL;

static void writeToScreen(const char * str, TlogLevel level =TlogLevel::LOG_VERBOSE)
{
    if(TlogSystem::get ()->getPolicy () ==TlogPolicy::CONSOLE || TlogSystem::get ()->getPolicy () ==TlogPolicy::CONSOLE_FILE)
    {
        if((int)level >= (int)TlogSystem::get ()->getLevel ())
        {
            if(TlogSystem::get ()->getWriteFunc ())
            {
                TlogSystem::get ()->getWriteFunc ()(str);
            }else
            {
                printf(str);
            }
        }
    }
}
static void writeToFile(const char * str, TlogLevel level =TlogLevel::LOG_VERBOSE)
{
    if(TlogSystem::get ()->getPolicy () ==TlogPolicy::FILE || TlogSystem::get ()->getPolicy () ==TlogPolicy::CONSOLE_FILE)
    {
        if((int)level >= (int)TlogSystem::get ()->getLevel ())
        {
            fprintf(TlogSystem::get ()->getFileHandle (),str);
        }

    }
}

Tlog::Tlog(TlogLevel level)
    :m_level(level),m_isFlushed(false)
{
    if(!TlogSystem::preTlog)
    {
        TlogSystem::preTlog = this;
    }
    else
    {
        TlogSystem::preTlog->flush();
    }
}

Tlog::~Tlog()
{
    flush();
	if(TlogSystem::preTlog == this)
	{
		TlogSystem::preTlog = nullptr;
	}
}

Tlog &Tlog::operator <<(const char *str)
{

    sprintf((char *)tmpStr,"%s",str);
    m_buffer.append (tmpStr);
    return (* this);
}

Tlog &Tlog::operator <<(const float f)
{
    sprintf((char *)tmpStr,"%g",f);
    m_buffer.append (tmpStr);
    return (* this);
}

Tlog &Tlog::operator <<(const int i)
{
    sprintf((char *)tmpStr,"%d",i);
    m_buffer.append (tmpStr);
    return (* this);
}

Tlog &Tlog::operator <<(const char c)
{
    sprintf((char *)tmpStr,"%c",c);
    m_buffer.append (tmpStr);
    return (* this);
}

Tlog &Tlog::operator <<(const double d)
{
    sprintf((char *)tmpStr,"%g",d);
    m_buffer.append (tmpStr);
    return (* this);
}

Tlog &Tlog::operator <<(const long l)
{
    sprintf((char *)tmpStr,"%ld",l);
    m_buffer.append (tmpStr);
    return (* this);
}

Tlog &Tlog::operator <<(const bool b)
{
    if(b)
    {
        sprintf ((char *)tmpStr,"true");
    }else
    {
        sprintf ((char *)tmpStr,"false");
    }
    m_buffer.append (tmpStr);
    return (* this);
}

Tlog &Tlog::operator <<(const std::string &str)
{
    sprintf ((char *)tmpStr,"%s",str.c_str ());
    m_buffer.append (tmpStr);
    return (* this);
}

Tlog &Tlog::operator <<(const Tstring &str)
{
    sprintf((char *)tmpStr,"%s",str.data ());
    m_buffer.append (tmpStr);
    return (* this);
}

Tlog &Tlog::operator <<(const TlogTitleEnd &el)
{
    (void)el;
    sprintf ((char *)tmpStr,"\n---*\n");
    m_buffer.append (tmpStr);
    return (* this);
}

Tlog &Tlog::operator <<(const TlogTitleBegin &begin)
{
    (void)begin;
    time_t rawtime;
	time ( &rawtime );
    struct tm * timeinfo = localtime ( &rawtime );
    sprintf ((char *)tmpStr,"\n--- %s",asctime (timeinfo));
    m_buffer.append (tmpStr);
    return (* this);
}

void Tlog::flush()
{
    if(!m_isFlushed)
    {
        //m_buffer.append ("\n");//append a new line in the end.
        writeToScreen(m_buffer.c_str ());
        writeToFile ((char *)m_buffer.c_str ());
        m_isFlushed =!m_isFlushed;
    }
}

TlogSystem *TlogSystem::get()
{
    if(!TlogSystem::system)
    {
        TlogSystem::system = new TlogSystem();
    }
    return TlogSystem::system;
}

TlogPolicy TlogSystem::getPolicy() const
{
    return m_policy;
}

void TlogSystem::setPolicy(const TlogPolicy &policy)
{
    m_policy = policy;
}

TlogSystem::TlogSystem()
    :m_policy(TlogPolicy::CONSOLE),m_level(TlogLevel::LOG_VERBOSE)
    ,m_writeFunc(nullptr)
{
    m_fileHandle = fopen("log/log.txt","w+");
}

TlogLevel TlogSystem::getLevel() const
{
    return m_level;
}

void TlogSystem::setLevel(const TlogLevel &level)
{
    m_level = level;
}

void TlogSystem::setWriteFunc(writeFunc func)
{
    m_writeFunc = func;
}

writeFunc TlogSystem::getWriteFunc()
{
    return m_writeFunc;
}

FILE *TlogSystem::getFileHandle() const
{
    return m_fileHandle;
}

void TlogSystem::setFileHandle(FILE *fileHandle)
{
    m_fileHandle = fileHandle;
}
} // namespace tzw

