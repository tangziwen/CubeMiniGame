#pragma once
#include "TinaCompiler.h"
#include "TinaValue.h"
#include <stack>
namespace tzw
{

struct FunctionEntryInfo
{
	size_t pc;
	size_t argCount;
};
class TinaRunTime
{
public:
	void execute(TinaProgram * program, std::string functionName);
	TinaVal refFromEnv(std::string identifier);
	TinaVal refFromStack(int stackID);
private:
	size_t m_PC = 0;//指令指针
	size_t m_SBP = 0;//调用栈基指针
	size_t m_SP = 0;//栈顶指针
	TinaVal m_register[32];
	TinaVal m_stack[1024];
	TinaVal m_retReg;//简化问题，特殊的寄存器存放返回值
	//偷个懒，函数地址单独存个栈 TODO
	std::stack<FunctionEntryInfo> m_funcAddrStack;
	void getVal(TinaProgram* program, OperandLocation location, TinaVal * val);
	std::unordered_map<std::string, TinaVal*> m_envMap;
};
}
