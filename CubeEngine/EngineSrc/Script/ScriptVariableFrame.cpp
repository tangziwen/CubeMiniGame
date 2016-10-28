#include "ScriptVariableFrame.h"

namespace tzw {

ScriptVariableFrame::ScriptVariableFrame()
{

}

ScriptValue ScriptVariableFrame::lookUp(std::string name)
{
	auto result = m_framePool.find(name);
	if (result != m_framePool.end())
	{
		return ScriptValue(&m_framePool[name]);
	}
	return ScriptValue(ScriptValue::Type::Impossible);
}

ScriptValue ScriptVariableFrame::setUp(std::string name, const ScriptValue &value)
{

	m_framePool[name] = value;
	return ScriptValue(&m_framePool[name]);
}

ScriptValue ScriptVariableFrame::define(std::string name)
{
	m_framePool[name] = ScriptValue(ScriptValue::Type::Null);
	return ScriptValue(&m_framePool[name]);
}
} // namespace tzw
