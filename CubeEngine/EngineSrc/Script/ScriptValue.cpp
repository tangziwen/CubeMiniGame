#include "ScriptValue.h"

namespace tzw {

ScriptValue::ScriptValue()
{
    m_type = Type::Null;
}

ScriptValue::ScriptValue(int theValue)
{
    m_type = Type::Integer;
    m_i = theValue;
}

ScriptValue::ScriptValue(std::string str)
{
    m_type = Type::Str;
    m_s = str;
}

ScriptValue::ScriptValue(ScriptValue::Type theType)
{
    m_type = theType;
}

void ScriptValue::initWithSymbol(std::string str)
{
    m_type = Type::Symbol;
    m_s = str;
}

int ScriptValue::i() const
{
    return m_i;
}

void ScriptValue::setI(int i)
{
    m_i = i;
}

std::string ScriptValue::s() const
{
    return m_s;
}

void ScriptValue::setS(const std::string &s)
{
    m_s = s;
}

float ScriptValue::r() const
{
    return m_r;
}

void ScriptValue::setR(float r)
{
    m_r = r;
}

bool ScriptValue::b() const
{
    return m_b;
}

void ScriptValue::setB(bool b)
{
    m_b = b;
}

ScriptValue::Type ScriptValue::type() const
{
    return m_type;
}

void ScriptValue::setType(const Type &type)
{
    m_type = type;
}

void ScriptValue::print()
{
    switch(m_type)
    {
    case Type::Integer:
        printf("%d\n",m_i);
        break;
    case Type::Real:
        printf("%f\n",m_r);
        break;
    }
}

ScriptValue ScriptValue::operator +(const ScriptValue &varB)
{
    if(m_type != varB.m_type)
    {
        return ScriptValue();
    }
    switch(m_type)
    {
    case Type::Integer:
        return m_i + varB.i();
    case Type::Real:
        return m_r + varB.r();
    }
}

ScriptValue ScriptValue::operator -(const ScriptValue &varB)
{
    if(m_type != varB.m_type)
    {
        return ScriptValue();
    }
    switch(m_type)
    {
    case Type::Integer:
        return m_i - varB.i();
    case Type::Real:
        return m_r - varB.r();
    }
}

ScriptValue ScriptValue::operator *(const ScriptValue &varB)
{
    if(m_type != varB.m_type)
    {
        return ScriptValue();
    }
    switch(m_type)
    {
    case Type::Integer:
        return m_i * varB.i();
    case Type::Real:
        return m_r * varB.r();
    }
}

ScriptValue ScriptValue::operator /(const ScriptValue &varB)
{
    if(m_type != varB.m_type)
    {
        return ScriptValue();
    }
    switch(m_type)
    {
    case Type::Integer:
        return m_i / varB.i();
    case Type::Real:
        return m_r / varB.r();
    }
}

bool ScriptValue::isConstantIteral()
{
    return (m_type == Type::Integer || m_type == Type::Real || m_type == Type::Str || m_type == Type::Null);
}
} // namespace tzw
