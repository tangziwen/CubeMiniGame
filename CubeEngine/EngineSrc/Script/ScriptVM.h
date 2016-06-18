#ifndef TZW_SCRIPTVM_H
#define TZW_SCRIPTVM_H
#include <stack>
#include <map>
#include <string>
#include "ScriptByteCode.h"
#include "ScriptVariableFrame.h"
namespace tzw {


class ScriptVM
{
public:
    ScriptVM();
    void excute(std::string procedrueName);
    void loadFromStr(std::string str,std::string procedrueName);
    ScriptValue lookUpSymbol(std::string variableName);
    ScriptValue setSymbol(std::string varialbleName, const ScriptValue &value);
    void defineGlobal(std::string name,ScriptValue initValue);
    void setGlobal(std::string name,ScriptValue newValue);
private:
    std::stack<ScriptValue> m_evalStack;
    std::map<std::string, ScriptByteCode * >m_byteCodeList;
    std::vector<ScriptVariableFrame> m_frameStack;
};

} // namespace tzw

#endif // TZW_SCRIPTVM_H
