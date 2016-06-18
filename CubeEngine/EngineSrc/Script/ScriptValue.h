#ifndef TZW_SCRIPTVAR_H
#define TZW_SCRIPTVAR_H

#include <string>
namespace tzw {

class ScriptValue
{
public:
    enum class Type
    {
        Integer,
        Ref,
        Real,
        Boolean,
        Null,
        Str,
        Symbol,
    };
    ScriptValue();
    ScriptValue(int theValue);
    ScriptValue(std::string str);
    ScriptValue(Type theType);

    void initWithSymbol(std::string str);
    int i() const;
    void setI(int i);

    std::string s() const;
    void setS(const std::string &s);

    float r() const;
    void setR(float r);

    bool b() const;
    void setB(bool b);

    Type type() const;
    void setType(const Type &type);
    void print();
    ScriptValue operator + (const ScriptValue & varB);
    ScriptValue operator - (const ScriptValue & varB);
    ScriptValue operator * (const ScriptValue & varB);
    ScriptValue operator / (const ScriptValue & varB);
    bool isConstantIteral();
private:
    int m_i;
    std::string m_s;
    float m_r;
    bool m_b;
    Type m_type;
};

} // namespace tzw

#endif // TZW_SCRIPTVAR_H
