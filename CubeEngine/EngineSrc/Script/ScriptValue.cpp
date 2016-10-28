#include "ScriptValue.h"
#include "ScriptStruct.h"
#include <math.h>
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

ScriptValue::ScriptValue(bool theValue)
{
	m_type = Type::Boolean;
	m_b = theValue;
}

ScriptValue::ScriptValue(float theValue)
{
	m_type = Type::Real;
	m_r = theValue;
}

ScriptValue::ScriptValue(std::string str)
{
	m_type = Type::Str;
	m_s = str;
}

ScriptValue::ScriptValue(void * usrPtr)
{
	m_type = Type::UserPtr;
	m_usrPtr = usrPtr;
}

ScriptValue::ScriptValue(const std::function<void (ScriptVM *)> & userFunction)
{
	m_type = Type::UserFunction;
	m_userFunction = userFunction;
}

ScriptValue::ScriptValue(ScriptValue::Type theType)
{
	m_type = theType;
}

ScriptValue::ScriptValue(ScriptStruct * structPtr)
{
	m_type = Type::structPtr;
	m_structPtr = structPtr;
}

ScriptValue::ScriptValue(ScriptValue * location)
{
	m_type = Type::LValue;
	m_refValue = location;
}

void ScriptValue::initWithSymbol(std::string str)
{
	m_type = Type::Symbol;
	m_s = str;
}

void ScriptValue::initWithScriptFunc(unsigned int str)
{
	m_type = Type::ScriptFunc;
	m_i = str;
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

bool ScriptValue::operator ==(const ScriptValue & varB)
{
	if(m_type != varB.m_type) return false;
	switch(m_type)
	{
		case Type::Integer:
		return m_i == varB.i();
		case Type::Real:
		return fabs(m_r - varB.r()) < 0.000001;
		case Type::Boolean:
			return m_b == varB.b();
	}
	return false;
}

bool ScriptValue::operator !=(const ScriptValue & varB)
{
	if(m_type != varB.m_type) return true;
	switch(m_type)
	{
		case Type::Integer:
		return m_i != varB.i();
		case Type::Real:
		return fabs(m_r - varB.r()) >= 0.000001;
		case Type::Boolean:
			return m_b != varB.b();
	}
	return false;
}

bool ScriptValue::operator >(const ScriptValue & varB)
{
	if(m_type != varB.m_type) return false;
	switch(m_type)
	{
		case Type::Integer:
		return m_i > varB.i();
		case Type::Real:
		return m_r > varB.r();
	}
	return false;
}

bool ScriptValue::operator <(const ScriptValue & varB)
{
	if(m_type != varB.m_type) return false;
	switch(m_type)
	{
		case Type::Integer:
		return m_i < varB.i();
		case Type::Real:
		return m_r < varB.r();
	}
	return false;
}

bool ScriptValue::operator >=(const ScriptValue & varB)
{
	if(m_type != varB.m_type) return false;
	switch(m_type)
	{
		case Type::Integer:
		return m_i >= varB.i();
		case Type::Real:
		return m_r >= varB.r();
	}
	return false;
}

bool ScriptValue::operator <=(const ScriptValue & varB)
{
	if(m_type != varB.m_type) return false;
	switch(m_type)
	{
		case Type::Integer:
		return m_i <= varB.i();
		case Type::Real:
		return m_r <= varB.r();
	}
	return false;
}

bool ScriptValue::isRValueType() const
{
	return (m_type == Type::Integer || m_type == Type::Real || m_type == Type::Str || m_type == Type::Boolean
			|| m_type == Type::UserPtr || m_type == Type::ScriptFunc);
}

std::string ScriptValue::toStr()
{
	char tmpStr[32] = "Impossible";
	switch(m_type)
	{
		case Type::Integer:
			sprintf(tmpStr,"%d",m_i);
		break;
		case Type::Real:
			sprintf(tmpStr,"%g",m_r);
		break;
		case Type::Str:
			sprintf(tmpStr,"%s",m_s.c_str());
		break;
		case Type::Null:
			sprintf(tmpStr,"Null");
		break;
		case Type::Boolean:
			if(m_b)
			{
				sprintf(tmpStr,"true");
			}else
			{
				sprintf(tmpStr,"false");
			}
		break;
		case Type::Impossible:
			sprintf(tmpStr, "Impossible");
		break;
		case Type::Ref:
			sprintf(tmpStr, "Ref");
		break;
		case Type::Symbol:
			sprintf(tmpStr,"%s(Symbol)",m_s.c_str());
		break;
		case Type::UserFunction:
			sprintf(tmpStr,"(userFunction)");
		break;
		case Type::ScriptFunc:
			sprintf(tmpStr,"(scriptFunc)");
		break;
		case Type::UserPtr:
			sprintf(tmpStr,"%p(usrPtr)",m_usrPtr);
		break;
	}
	return tmpStr;
}

std::function<void (ScriptVM *)> ScriptValue::getUserFunction() const
{
	return m_userFunction;
}

void ScriptValue::setUserFunction(const std::function<void (ScriptVM *)> & userFunction)
{
	m_type = Type::UserFunction;
	m_userFunction = userFunction;
}

ScriptStruct * ScriptValue::getStructPtr() const
{
	return m_structPtr;
}

ScriptValue * ScriptValue::getRefValue() const
{
	return m_refValue;
}

ScriptValue ScriptValue::toRV() const
{
	if(isRValueType())
	{
		return (*this);
	}
	else if(type() == ScriptValue::Type::LValue)
	{
		return (*getRefValue());
	}
	return ScriptValue(ScriptValue::Type::Impossible);
}
} // namespace tzw
