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
        return result->second;
    }else
    {
        return ScriptValue(ScriptValue::Type::Null);
    }
}

ScriptValue ScriptVariableFrame::setUp(std::string name, ScriptValue &value)
{
    auto result = m_framePool.find(name);
    if (result != m_framePool.end())
    {
        m_framePool[name] = value;
    }else
    {
        printf("the variable %s is not decleared",name.c_str());
    }
    return ScriptValue(ScriptValue::Type::Null);
}

void ScriptVariableFrame::declear(std::string name)
{
    auto result = m_framePool.find(name);
    if (result != m_framePool.end())
    {
        printf("the name %s has been decleared",name.c_str());
    }
    m_framePool[name] = ScriptValue(ScriptValue::Type::Null);
}

} // namespace tzw
