#pragma once
#include <string>

namespace tzw
{

struct TinaFunctionRuntimeInfo
{
	char m_name[32];
	size_t m_entryAddr;
	size_t m_varCount;
};

enum class TinaValType
{
	Int,
	Float,
	String,
	Ref,
	FuncPtr,
	Boolean,
	Nil,
};
struct TinaVal
{
	TinaVal();
	~TinaVal();
	union 
	{
		int valI;
		float valF;
		char * valStr;
		TinaVal* valRef;//ptr
		TinaFunctionRuntimeInfo valFunctPtr;
	} m_data;
	TinaValType m_type;
	char * toStr();
};

TinaVal valAdd_impF(TinaVal * a, TinaVal * b);
TinaVal valAdd_impI(TinaVal * a, TinaVal * b);
TinaVal valAdd_impStr(TinaVal * a, TinaVal * b);
TinaVal valMul_impF(TinaVal * a, TinaVal * b);
TinaVal valMul_impI(TinaVal * a, TinaVal * b);
TinaVal valSub_impF(TinaVal * a, TinaVal * b);
TinaVal valSub_impI(TinaVal * a, TinaVal * b);
TinaVal valDiv_impF(TinaVal * a, TinaVal * b);
TinaVal valDiv_impI(TinaVal * a, TinaVal * b);
TinaVal valAdd(TinaVal * a, TinaVal * b);
TinaVal valMul(TinaVal * a, TinaVal * b);
TinaVal valSub(TinaVal * a, TinaVal * b);
TinaVal valDiv(TinaVal * a, TinaVal * b);
}
