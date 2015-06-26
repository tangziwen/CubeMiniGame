#ifndef TSTRING_H
#define TSTRING_H
#include <string>
namespace tzw {

class Tstring
{
public:
    Tstring();
    Tstring(const char * m);
    Tstring(const std::string& str);
    Tstring(const Tstring & str);

    //=
    const Tstring & operator =(const char * m);
    const Tstring & operator =(const Tstring & string);
    const Tstring & operator =(const std::string & string);
    // +
    const Tstring &operator +(const char *m)const;
    const Tstring &operator +(const std::string &m)const;
    const Tstring &operator +(const Tstring &m)const;

    const Tstring &operator +=(const char * m);
    const Tstring &operator +=(const Tstring & str);
    const Tstring &operator +=(const std::string & str);

    char *data() const;
    unsigned int size() const;
    void clear();
    void concat(const char * str);
    void concat(const std::string & str);
    void concat(const Tstring & str);
private:
    void initWithCharArray(const char * str);
    char * m_data;
    unsigned int m_size;
};
}// namespace tzw
#endif // TSTRING_H
