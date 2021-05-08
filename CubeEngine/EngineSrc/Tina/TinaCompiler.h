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
	CALL,
	RET,
	PRINT,
	HALT,
};



struct OperandLocation
{
	enum class locationType
	{
		REGISTER = 0,
		ENV,
		CONSTVAL,
		IMEEDIATE,
		INVALID,
		STACK,
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
	std::string m_name;
	size_t m_entryAddr;
};
struct TinaProgram
{
	TinaProgram()
	{
		
	};
	std::vector<std::string> strLiteral;//string literal, use for env variable look up, member look up etc.
	std::vector<std::string> stackVar;
	std::vector<TinaVal> constVal;
	std::vector<ILCmd> cmdList;
	std::vector<TinaFunctionInfo> functionInfoList;

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
	std::unordered_map<std::string, int> m_stackMap;
	std::unordered_map<std::string, int> m_envSymbolMap;
	//常量没有名字，直接放值
	std::unordered_map<std::string, int> m_constMap;
	OperandLocation getLeafAddr(TinaASTNode * node, TinaProgram & program);
	OperandLocation evalR(TinaASTNode * node, TinaProgram & program);
	OperandLocation evalL(TinaASTNode * node, TinaProgram & program);
	OperandLocation genTmpValue();
	void decreaseRegIndex(int count = 1);
};

}
