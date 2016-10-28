#ifndef TZW_SCRIPTVAR_H
#define TZW_SCRIPTVAR_H

#include <string>
#include <functional>
namespace tzw {
class ScriptVM;
class ScriptStruct;
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
		Impossible,
		UserFunction,
		UserPtr,
		structPtr,
		LValue,
		RawPtr,
		ScriptFunc,
	};
	ScriptValue();
	ScriptValue(int theValue);
	ScriptValue(bool theValue);
	ScriptValue(float theValue);
	ScriptValue(std::string str);
	ScriptValue(void * m_usrPtr);
	ScriptValue(const std::function<void (ScriptVM *)> & userFunction);
	ScriptValue(Type theType);
	ScriptValue(ScriptStruct * structPtr);
	ScriptValue(ScriptValue * location);

	void initWithSymbol(std::string str);
	void initWithScriptFunc(unsigned int str);
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
	bool operator == (const ScriptValue & varB);
	bool operator != (const ScriptValue & varB);
	bool operator > (const ScriptValue & varB);
	bool operator < (const ScriptValue & varB);
	bool operator >= (const ScriptValue & varB);
	bool operator <= (const ScriptValue & varB);
	bool isRValueType() const;

	std::string toStr();
	std::function<void (ScriptVM *)> getUserFunction() const;
	void setUserFunction(const std::function<void (ScriptVM *)> & userFunction);
	ScriptStruct * getStructPtr() const;

	ScriptValue * getRefValue() const;
	//转换成右值，对于本身是右值的，没有任何影响，左值需要进行一次寻址
	ScriptValue toRV() const;

private:
	std::function<void (ScriptVM *)> m_userFunction;
	int m_i;
	std::string m_s;
	float m_r;
	bool m_b;
	Type m_type;
	void * m_usrPtr;
	ScriptStruct * m_structPtr;
	ScriptValue * m_refValue;
};

} // namespace tzw

#endif // TZW_SCRIPTVAR_H
