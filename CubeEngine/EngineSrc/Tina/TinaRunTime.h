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
	size_t m_PC = 0;//ָ��ָ��
	size_t m_SBP = 0;//����ջ��ָ��
	size_t m_SP = 0;//ջ��ָ��
	TinaVal m_register[32];
	TinaVal m_stack[1024];
	TinaVal m_retReg;//�����⣬����ļĴ�����ŷ���ֵ
	//͵������������ַ�������ջ TODO
	std::stack<FunctionEntryInfo> m_funcAddrStack;
	void getVal(TinaProgram* program, OperandLocation location, TinaVal * val);
	std::unordered_map<std::string, TinaVal*> m_envMap;
};
}
