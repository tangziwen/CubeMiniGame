#include "ScriptStruct.h"
#include "ScriptValue.h"
namespace tzw {

ScriptStruct::ScriptStruct()
{

}

ScriptValue * ScriptStruct::get(std::string name)
{
	if(m_valueMap.find(name) == m_valueMap.end())
	{
		auto theNewVal = new ScriptValue(ScriptValue::Type::Null);
		m_valueMap[name] = theNewVal;
		return theNewVal;
	}
	return m_valueMap[name];
}

ScriptValue *ScriptStruct::getByIndex(int index)
{
	if(index + 1 > m_valueList.size())
	{
		m_valueList.resize(index);
		m_valueList[index] = new ScriptValue(ScriptValue::Type::Null);
		return m_valueList[index];
	}
	return m_valueList[index];
}

void ScriptStruct::append(ScriptValue * value)
{
	m_valueList.push_back(value);
}

void ScriptStruct::set(std::string name, ScriptValue * value)
{
	m_valueMap[name] = value;
}

void ScriptStruct::set(int index, ScriptValue * value)
{
	delete m_valueList[index];
	m_valueList[index] = value;
}

} // namespace tzw
