#pragma once
#include "TinaCompiler.h"
#include "TinaValue.h"
namespace tzw
{

class TinaRunTime
{
public:
	void execute(TinaProgram * program);
	TinaVal refFromEnv(std::string identifier);
	TinaVal refFromStack(int stackID);
private:
	size_t m_PC = 0;
	size_t m_SBP = 0;
	TinaVal m_register[32];
	TinaVal m_stack[256];
	void getVal(TinaProgram* program, OperandLocation location, TinaVal * val);
	std::unordered_map<std::string, TinaVal*> m_envMap;
};
}
