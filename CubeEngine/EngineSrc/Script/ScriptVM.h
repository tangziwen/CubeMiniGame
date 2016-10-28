#ifndef TZW_SCRIPTVM_H
#define TZW_SCRIPTVM_H
#include <stack>
#include <map>
#include <string>
#include "ScriptByteCode.h"
#include "ScriptVariableFrame.h"
#include "ScriptStruct.h"
#include "Tokenizer.h"
#include <deque>
namespace tzw {


class ScriptVM
{
public:
    ScriptVM();
    void excute(unsigned int procedrueName);
    void loadFromStr(std::string str, unsigned int procedrueName);
	int evalFromTKList(std::vector<ScriptToken> tkList, int index, unsigned int procedrueName);
	void loadFromFile(std::string fileName, unsigned int procedrueName);

    ScriptValue lookUpLocal(std::string variableName);
    ScriptValue setLocal(std::string varialbleName, const ScriptValue &value);
	ScriptValue defineLocal(std::string variableName);

	ScriptValue lookUpGlobal(std::string variableName);
	void defineGlobal(std::string variableName);
    void setGlobal(std::string name,ScriptValue newValue);

	ScriptValue userStackPop();
	void userStackPush(ScriptValue value);
	void userStackClear();

	void defineFunction(std::string funcName, std::function<void(ScriptVM*)> func);
	void useStdLibrary();
	void addHeapObj(ScriptStruct * theStruct);
	unsigned int getAvailableFuncID();
	void incFrame();
	void decFrame();
	ScriptByteCode * getByteCode(unsigned int funcID);
private:
    std::stack<ScriptValue> m_evalStack;
	std::stack<ScriptValue> m_userStack;
    std::map<unsigned int, ScriptByteCode * >m_byteCodeList;
    std::deque<ScriptVariableFrame> m_frameStack;
	std::vector<ScriptStruct *> m_heap;
	unsigned int m_funcID;
};

} // namespace tzw

#endif // TZW_SCRIPTVM_H
