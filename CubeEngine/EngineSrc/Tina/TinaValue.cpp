#include "TinaValue.h"

#include <cstdio>

namespace tzw
{
TinaVal::TinaVal()
{
}

TinaVal::~TinaVal()
{
}

char* TinaVal::toStr()
{
	static char tmp[256] ="";
	switch(m_type)
	{
	case TinaValType::Int:
		sprintf_s(tmp, "%d",m_data.valI);
		break;
	case TinaValType::Float:
		sprintf_s(tmp, "%f",m_data.valF);
		break;
	case TinaValType::String:
		sprintf_s(tmp, "%s",m_data.valStr);
		break;
	case TinaValType::Ref:
		sprintf_s(tmp, "Ref:%p",m_data.valRef);
		break;
	case TinaValType::Nil:
		sprintf_s(tmp, "Nil");
		break;
	default: ;
		
	}
	return tmp;
}

TinaVal valAdd_impF(TinaVal* a, TinaVal* b)
{
	TinaVal val;
	val.m_data.valF = a->m_data.valF + b->m_data.valF;
	val.m_type = TinaValType::Float;
	return val;
}

TinaVal valAdd_impI(TinaVal* a, TinaVal* b)
{
	TinaVal val;
	val.m_data.valI = a->m_data.valI + b->m_data.valI;
	val.m_type = TinaValType::Int;
	return val;
}

TinaVal valAdd_impStr(TinaVal* a, TinaVal* b)
{
	TinaVal val;
	size_t newStrSize = strlen(a->m_data.valStr) + strlen(b->m_data.valStr) + 1;
	char * newStr = (char *)malloc(newStrSize);
	strcpy(newStr, a->m_data.valStr);
	strcat(newStr, b->m_data.valStr);
	val.m_data.valStr = newStr;
	val.m_type = TinaValType::String;
	return val;
}

TinaVal valMul_impF(TinaVal* a, TinaVal* b)
{
	TinaVal val;
	val.m_data.valF = a->m_data.valF * b->m_data.valF;
	val.m_type = TinaValType::Float;
	return val;
}

TinaVal valMul_impI(TinaVal* a, TinaVal* b)
{
	TinaVal val;
	val.m_data.valI = a->m_data.valI * b->m_data.valI;
	val.m_type = TinaValType::Int;
	return val;
}

TinaVal valSub_impF(TinaVal* a, TinaVal* b)
{
	TinaVal val;
	val.m_data.valF = a->m_data.valF - b->m_data.valF;
	val.m_type = TinaValType::Float;
	return val;
}

TinaVal valSub_impI(TinaVal* a, TinaVal* b)
{
	TinaVal val;
	val.m_data.valF = a->m_data.valI - b->m_data.valI;
	val.m_type = TinaValType::Int;
	return val;
}

TinaVal valDiv_impF(TinaVal* a, TinaVal* b)
{
	TinaVal val;
	val.m_data.valF = a->m_data.valF / b->m_data.valF;
	val.m_type = TinaValType::Float;
	return val;
}

TinaVal valDiv_impI(TinaVal* a, TinaVal* b)
{
	TinaVal val;
	val.m_data.valF = a->m_data.valI + b->m_data.valI;
	val.m_type = TinaValType::Int;
	return val;
}

TinaVal valGreater_impF(TinaVal* a, TinaVal* b)
{
	TinaVal val;
	val.m_data.valI = a->m_data.valF > b->m_data.valF;
	val.m_type = TinaValType::Boolean;
	return val;
}

TinaVal valGreater_impI(TinaVal* a, TinaVal* b)
{
	TinaVal val;
	val.m_data.valI = a->m_data.valI > b->m_data.valI;
	val.m_type = TinaValType::Boolean;
	return val;
}


TinaVal valGreaterOREqual_impF(TinaVal* a, TinaVal* b)
{
	TinaVal val;
	val.m_data.valI = a->m_data.valF >= b->m_data.valF;
	val.m_type = TinaValType::Boolean;
	return val;
}

TinaVal valGreaterOREqual_impI(TinaVal* a, TinaVal* b)
{
	TinaVal val;
	val.m_data.valI = a->m_data.valI >= b->m_data.valI;
	val.m_type = TinaValType::Boolean;
	return val;
}

TinaVal valLess_impF(TinaVal* a, TinaVal* b)
{
	TinaVal val;
	val.m_data.valI = a->m_data.valF < b->m_data.valF;
	val.m_type = TinaValType::Boolean;
	return val;
}

TinaVal valLess_impI(TinaVal* a, TinaVal* b)
{
	TinaVal val;
	val.m_data.valI = a->m_data.valI < b->m_data.valI;
	val.m_type = TinaValType::Boolean;
	return val;
}

TinaVal valLessOrEqual_impF(TinaVal* a, TinaVal* b)
{
	TinaVal val;
	val.m_data.valI = a->m_data.valF <= b->m_data.valF;
	val.m_type = TinaValType::Boolean;
	return val;
}

TinaVal valLessOrEqual_impI(TinaVal* a, TinaVal* b)
{
	TinaVal val;
	val.m_data.valI = a->m_data.valI <= b->m_data.valI;
	val.m_type = TinaValType::Boolean;
	return val;
}

TinaVal valEqual_impF(TinaVal* a, TinaVal* b)
{
	TinaVal val;
	val.m_data.valI = a->m_data.valF == b->m_data.valF;
	val.m_type = TinaValType::Boolean;
	return val;
}

TinaVal valEqual_impI(TinaVal* a, TinaVal* b)
{
	TinaVal val;
	val.m_data.valI = a->m_data.valI == b->m_data.valI;
	val.m_type = TinaValType::Boolean;
	return val;
}

TinaVal valNotEqual_impF(TinaVal* a, TinaVal* b)
{
	TinaVal val;
	val.m_data.valI = a->m_data.valF != b->m_data.valF;
	val.m_type = TinaValType::Boolean;
	return val;
}

TinaVal valNotEqual_impI(TinaVal* a, TinaVal* b)
{
	TinaVal val;
	val.m_data.valI = a->m_data.valI != b->m_data.valI;
	val.m_type = TinaValType::Boolean;
	return val;
}

TinaVal valAdd(TinaVal* a, TinaVal* b)
{
	if(a->m_type == b->m_type)// life is so much easier
	{
		if(a->m_type == TinaValType::Float)
		{
			return valAdd_impF(a, b);
		}
		else if(a->m_type == TinaValType::Int)
		{
			return valAdd_impI(a, b);
		}
		else if(a->m_type == TinaValType::String)
		{
			return valAdd_impStr(a, b);
		}
		
	}else//dirty type boosting, fuck you coder.
	{

	}
}

TinaVal valMul(TinaVal* a, TinaVal* b)
{
	if(a->m_type == b->m_type)// life is so much easier
	{
		if(a->m_type == TinaValType::Float)
		{
			return valMul_impF(a, b);
		}
		else if(a->m_type == TinaValType::Int)
		{
			return valMul_impI(a, b);
		}
		
	}else//dirty type boosting, fuck you coder.
	{

	}
}

TinaVal valSub(TinaVal* a, TinaVal* b)
{
	if(a->m_type == b->m_type)// life is so much easier
	{
		if(a->m_type == TinaValType::Float)
		{
			return valSub_impF(a, b);
		}
		else if(a->m_type == TinaValType::Int)
		{
			return valSub_impI(a, b);
		}
		
	}else//dirty type boosting, fuck you coder.
	{

	}
}

TinaVal valDiv(TinaVal* a, TinaVal* b)
{
	if(a->m_type == b->m_type)// life is so much easier
	{
		if(a->m_type == TinaValType::Float)
		{
			return valDiv_impF(a, b);
		}
		else if(a->m_type == TinaValType::Int)
		{
			return valDiv_impI(a, b);
		}
		
	}else//dirty type boosting, fuck you coder.
	{

	}
}
TinaVal valGreater(TinaVal* a, TinaVal* b)
{
	if(a->m_type == b->m_type)// life is so much easier
	{
		if(a->m_type == TinaValType::Float)
		{
			return valGreater_impF(a, b);
		}
		else if(a->m_type == TinaValType::Int)
		{
			return valGreater_impI(a, b);
		}
		
	}else//dirty type boosting, fuck you coder.
	{

	}
}
TinaVal valGreaterOREqual(TinaVal* a, TinaVal* b)
{
	if(a->m_type == b->m_type)// life is so much easier
	{
		if(a->m_type == TinaValType::Float)
		{
			return valGreaterOREqual_impF(a, b);
		}
		else if(a->m_type == TinaValType::Int)
		{
			return valGreaterOREqual_impI(a, b);
		}
		
	}else//dirty type boosting, fuck you coder.
	{

	}
}
TinaVal valLess(TinaVal* a, TinaVal* b)
{
	if(a->m_type == b->m_type)// life is so much easier
	{
		if(a->m_type == TinaValType::Float)
		{
			return valLess_impF(a, b);
		}
		else if(a->m_type == TinaValType::Int)
		{
			return valLess_impI(a, b);
		}
		
	}else//dirty type boosting, fuck you coder.
	{

	}
}
TinaVal valLessOrEqual(TinaVal* a, TinaVal* b)
{
	if(a->m_type == b->m_type)// life is so much easier
	{
		if(a->m_type == TinaValType::Float)
		{
			return valLessOrEqual_impF(a, b);
		}
		else if(a->m_type == TinaValType::Int)
		{
			return valLessOrEqual_impI(a, b);
		}
		
	}else//dirty type boosting, fuck you coder.
	{

	}
}
TinaVal valEqual(TinaVal* a, TinaVal* b)
{
	if(a->m_type == b->m_type)// life is so much easier
	{
		if(a->m_type == TinaValType::Float)
		{
			return valEqual_impF(a, b);
		}
		else if(a->m_type == TinaValType::Int)
		{
			return valEqual_impI(a, b);
		}
		
	}else//dirty type boosting, fuck you coder.
	{

	}
}

TinaVal valNotEqual(TinaVal* a, TinaVal* b)
{
	if(a->m_type == b->m_type)// life is so much easier
	{
		if(a->m_type == TinaValType::Float)
		{
			return valNotEqual_impF(a, b);
		}
		else if(a->m_type == TinaValType::Int)
		{
			return valNotEqual_impI(a, b);
		}
		
	}else//dirty type boosting, fuck you coder.
	{

	}
}
}
