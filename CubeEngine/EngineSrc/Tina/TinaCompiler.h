#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include "TinaValue.h"
namespace tzw
{
struct TinaASTNode;

enum class ILCommandType
{
	MOV,
	MOVINDIRECT,
	LEA,
	PUSH,
	POP,
	ADD,
	SUB,
	MUL,
	DIV,
	JMP,
	JNE,
	JE,
	CALL,
	RET,
	PRINT,
	HALT,
	LOG_GRT,
	LOG_GE,
	LOG_LES,
	LOG_LE,
	LOG_EQ,
	LOG_NE
};



struct OperandLocation
{
	enum class locationType
	{
		INVALID = 0,
		REGISTER,
		ENV,
		CONSTVAL,
		IMEEDIATE,
		STACK,
		RETREG,
	};
	locationType m_locSrc = locationType::INVALID;
	unsigned char m_addr = 0;
	OperandLocation(locationType src, unsigned char addr): m_locSrc(src),m_addr(addr)
	{
		
	}
	OperandLocation()
	{
		
	}
};
//3-addresses code: C = A + B
struct ILCmd
{
	ILCommandType m_type;
	OperandLocation m_A;
	OperandLocation m_B;
	OperandLocation m_C;
	explicit ILCmd(ILCommandType type);
	ILCmd(ILCommandType type, OperandLocation A);
	ILCmd(ILCommandType type, OperandLocation A, OperandLocation B);
	ILCmd(ILCommandType type, OperandLocation A, OperandLocation B, OperandLocation C);
};

struct TinaFunctionInfo
{
	TinaFunctionRuntimeInfo m_rtInfo;
	std::vector<std::string> stackVar;
	std::unordered_map<std::string, int> m_stackMap;//for accelerate searching
};

struct TinaProgram
{
	TinaProgram()
	{
		
	};
	std::vector<std::string> strLiteral;//string literal, use for env variable look up, member look up etc.
	std::vector<TinaVal> constVal;
	std::vector<ILCmd> cmdList;
	std::vector<TinaFunctionInfo *> functionInfoList;
	std::unordered_map<std::string, TinaVal*> m_envMap;
	TinaFunctionInfo* findFunctionInfoFromName(std::string funcName);
};
//statk vars are early binding, we only save index
//env vars (upvaule) are late binding, we must look up in runtime
//properties access eg. A.B are late binding too, since the variable hase dynamic type. we store B to const list
class TinaCompiler
{
public:
	TinaProgram gen(TinaASTNode * astRootNode);
private:
	unsigned char m_registerIndex = 0;
	
	std::unordered_map<std::string, int> m_envSymbolMap;
	//常量没有名字，直接放值
	std::unordered_map<std::string, int> m_constMap;
	TinaFunctionInfo * m_currParsingFunc = nullptr;
	OperandLocation getLeafAddr(TinaASTNode * node, TinaProgram & program);
	OperandLocation evalR(TinaASTNode * node, TinaProgram & program);
	OperandLocation evalL(TinaASTNode * node, TinaProgram & program);
	OperandLocation genTmpValue();
	void decreaseRegIndex(int count = 1);
};

}
