#include "Tstring.h"
#include <stdlib.h>
#include <string.h>
namespace tzw {
Tstring::Tstring()
    :m_data(NULL),m_size(0)
{

}

Tstring::Tstring(const char *m)
{
    initWithCharArray(m);
}

Tstring::Tstring(const std::string& str)
{
    initWithCharArray(str.c_str ());
}

Tstring::Tstring(const Tstring &str)
{
    initWithCharArray (str.data ());
}

const Tstring &Tstring::operator =(const char *m)
{
    clear ();
    initWithCharArray (m);
    return *this;
}

const Tstring &Tstring::operator =(const Tstring &string)
{
    clear();
    initWithCharArray (string.data ());
	return string;
}

const Tstring &Tstring::operator =(const std::string &string)
{
    clear();
    initWithCharArray (string.c_str ());
	return string;
}

const Tstring &Tstring::operator +(const char *m) const
{
    Tstring a = (* this);
    a.concat (m);
    return a;
}

const Tstring &Tstring::operator +(const std::string &m)const
{
    Tstring a = (* this);
    a.concat (m.c_str ());
    return a;
}

const Tstring &Tstring::operator +(const Tstring &m) const
{
    Tstring a = (* this);
    a.concat (m.data ());
    return a;
}

const Tstring &Tstring::operator +=(const char *m)
{
    this->concat (m);
    return (* this);
}

const Tstring &Tstring::operator +=(const Tstring &str)
{
    this->concat (str);
    return (*this);
}

const Tstring &Tstring::operator +=(const std::string &str)
{
    this->concat (str);
    return (*this);
}

char *Tstring::data() const
{
    return m_data;
}

unsigned int Tstring::size() const
{
    return m_size;
}

void Tstring::clear()
{
    free(m_data);
    m_size = 0;
}

void Tstring::concat(const char *str)
{
    char * tmp = (char *)malloc(m_size*sizeof(char)+strlen (str) +sizeof(char));
    tmp[0] = NULL;// empty string
    strcat(tmp,m_data);
    strcat(tmp,str);
    free(m_data);
    m_data = tmp;
    m_size +=strlen(str);
}

void Tstring::concat(const std::string &str)
{
    this->concat (str.c_str ());
}

void Tstring::concat(const Tstring &str)
{
    this->concat (str.data ());
}

void Tstring::initWithCharArray(const char *str)
{
    m_data = (char *)malloc(strlen(str) + sizeof(char));
    memcpy (m_data,str,strlen(str)+sizeof(char));
    m_size = strlen (m_data);
}

}// namespace tzw
