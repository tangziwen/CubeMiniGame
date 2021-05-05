#pragma once
namespace tzw
{
enum class TinaValType
{
	Int,
	Float,
	Ref,
	Nil,
};
struct TinaVal
{
	union 
	{
		int valI;
		float valF;
		TinaVal* valRef;//ptr
	} m_data;
	TinaValType m_type;
	char * toStr();
};
TinaVal valAdd_impF(TinaVal * a, TinaVal * b);
TinaVal valAdd_impI(TinaVal * a, TinaVal * b);
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
