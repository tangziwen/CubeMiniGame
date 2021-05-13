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
TinaVal valGreater_impF(TinaVal * a, TinaVal * b);
TinaVal valGreater_impI(TinaVal * a, TinaVal * b);
TinaVal valGreaterOREqual_impF(TinaVal * a, TinaVal * b);
TinaVal valGreaterOREqual_impI(TinaVal * a, TinaVal * b);
TinaVal valLess_impF(TinaVal * a, TinaVal * b);
TinaVal valLess_impI(TinaVal * a, TinaVal * b);
TinaVal valLessOrEqual_impF(TinaVal * a, TinaVal * b);
TinaVal valLessOrEqual_impI(TinaVal * a, TinaVal * b);
TinaVal valEqual_impF(TinaVal * a, TinaVal * b);
TinaVal valEqual_impI(TinaVal * a, TinaVal * b);
TinaVal valNotEqual_impF(TinaVal * a, TinaVal * b);
TinaVal valNotEqual_impI(TinaVal * a, TinaVal * b);


TinaVal valAdd(TinaVal * a, TinaVal * b);
TinaVal valMul(TinaVal * a, TinaVal * b);
TinaVal valSub(TinaVal * a, TinaVal * b);
TinaVal valDiv(TinaVal * a, TinaVal * b);
TinaVal valGreater(TinaVal * a, TinaVal * b);
TinaVal valGreaterOREqual(TinaVal * a, TinaVal * b);
TinaVal valLess(TinaVal * a, TinaVal * b);
TinaVal valLessOrEqual(TinaVal * a, TinaVal * b);
TinaVal valEqual(TinaVal * a, TinaVal * b);
TinaVal valNotEqual(TinaVal * a, TinaVal * b);
}
