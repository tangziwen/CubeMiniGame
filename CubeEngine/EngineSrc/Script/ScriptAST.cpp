#include "ScriptAST.h"
#include <string.h>
#include "ScriptVM.h"
namespace tzw {

#define GET_TK (theList[m_index])

#define GET_OP (theList[m_index].m_str[0])

#define NEXT (m_index++)

#define NEXT_VALID ( !(m_index+1 >= theList.size()))

#define LABEL_WHILE_LOOP 2
#define LABEL_WHILE_BREAK 3

ScriptAST::ScriptAST()
{
	m_index = 0;
}

void ScriptAST::parseExpr(std::vector<ScriptToken> &theList, ScriptByteCode *byteCode)
{
	m_currByteCode = byteCode;
	auto firstToken = theList[m_index];
	if(firstToken.isKeyWord("var"))
	{
		NEXT;
		evalDefine(theList);

	}
	else if(firstToken.isKeyWord("if"))
	{
		NEXT;
		evalIf(theList);
	}
	else if(firstToken.isKeyWord("while"))
	{
		NEXT;
		evalWhile(theList);
	}
	else if(firstToken.isKeyWord("break"))
	{
		NEXT;
		m_currByteCode->JMP(LABEL_WHILE_BREAK);
	}
	else if(firstToken.isKeyWord("return"))
	{
		NEXT;
		if(getTk(theList,';'))//直接返回
		{
			m_currByteCode->Return(false);
		}
		else//带返回值的返回
		{
			evalExpr0(theList);
			m_currByteCode->Return(true);
		}
	}
	else if(firstToken.m_str =="{")
	{
		evalBlock(theList);
	}
	else
	{
		evalExpr0(theList);
	}

}

void ScriptAST::evalTerm(std::vector<ScriptToken> &theList)
{
	evalSubTerm(theList);
	if (getTk(theList,'*'))
	{
		NEXT;
		evalTerm(theList);
		m_currByteCode->multiple();
	}
	else if(getTk(theList,'/'))
	{
		NEXT;
		evalTerm(theList);
		m_currByteCode->divide();
	}
	else if(getTk(theList,';'))
	{
		return;
	}
}

void ScriptAST::evalSubTerm(std::vector<ScriptToken> & theList)
{
	evalFactor(theList);
	if (getTk(theList,'.'))
	{
		NEXT;
		auto memberTK = GET_TK;
		if(memberTK.m_type != ScriptToken::Type::Symbol)
		{
			printf("member name must be symbol");
			exit(0);
		}

		//evalSubTerm(theList);
		m_currByteCode->getMember(memberTK.m_str);
		NEXT;
	}
	else if(getTk(theList,';'))
	{
		return;
	}
}

void ScriptAST::evalFactor(std::vector<ScriptToken> &theList)
{
	auto tk = GET_TK;
	if (tk.isOp('('))//括号装起来的表达式
	{
		NEXT;
		evalExpr0(theList);
		tk = GET_TK;
		if(tk.isOp(')'))
		{
			NEXT;//略过右括号
		}else
		{
			printf("fatal error!\n");
			return;
		}
	}
	else if(tk.m_type == ScriptToken::Type::Integer)//数值
	{
		auto theValue = GET_TK.m_integer;
		m_currByteCode->pushIteral(ScriptValue(theValue));
		NEXT;
	}
	else if(tk.m_type == ScriptToken::Type::Real)//数值
	{
		auto theValue = GET_TK.m_real;
		m_currByteCode->pushIteral(ScriptValue(theValue));
		NEXT;
	}
	else if(tk.m_type == ScriptToken::Type::Boolean)//数值
	{
		auto theValue = GET_TK.m_boolean;
		m_currByteCode->pushIteral(ScriptValue(theValue));
		NEXT;
	}
	else if(tk.m_type == ScriptToken::Type::Str)//数值
	{
		auto theValue = GET_TK.m_str;
		m_currByteCode->pushIteral(ScriptValue(theValue));
		NEXT;
	}
	//函数形式...
	else if(tk.isKeyWord("function"))
	{
		//skip 'function' keyword.
		NEXT;
		//skip parameters.
		NEXT;
		std::vector<std::string> arglist;
		while(!GET_TK.isOp(')'))
		{
			auto tk = GET_TK;
			if(tk.m_type == ScriptToken::Type::Symbol)
			{
				arglist.push_back(tk.m_str);
			}
			else if(tk.isOp(','))
			{
				//do nothing with comma.
			}
			else
			{
				printf("error, invalid function parameter list");
				exit(0);
			}
			NEXT;
		}
		NEXT;
		auto funcID = m_vm->getAvailableFuncID();
		m_index = m_vm->evalFromTKList(theList, m_index, funcID);
		m_vm->getByteCode(funcID)->m_argList = arglist;
		NEXT;//跳过右括号.
		ScriptValue val;
		val.initWithScriptFunc(funcID);
		m_currByteCode->pushIteral(val);
	}
	//是一个符号,那么这个符号要么用来进行函数调用,要么是一个变量名,要根据符号解引用到值.
	else if(tk.m_type == ScriptToken::Type::Symbol)
	{
		if(NEXT_VALID && theList[m_index+1].isOp('('))//调用形式
		{
			NEXT;//左括号
			NEXT;
			ScriptToken argTK = GET_TK;
			std::vector<int> argList;
			int argCount = 0;
			while(!argTK.isOp(')'))
			{
				argTK = GET_TK;
				if(!argTK.isOp(','))
				{
					evalExpr0(theList);
					argCount ++;
					argTK = GET_TK;
					//						argList.push_back(0);//参数列表
					//						NEXT;
				}else
				{
					NEXT;
					argTK = GET_TK;
				}
			}
			NEXT;
			ScriptValue symbolValue;
			symbolValue.initWithSymbol(tk.m_str);
			m_currByteCode->Call(symbolValue,argCount);
		}else//变量形式
		{
			ScriptValue value;
			m_currByteCode->lookUp(tk.m_str);
			NEXT;
		}

	}
}

void ScriptAST::evalExpr1(std::vector<ScriptToken> &theList)
{
	evalTerm(theList);
	if (getTk(theList,'+'))
	{
		NEXT;
		evalExpr1(theList);
		m_currByteCode->add();
	}
	else if (getTk(theList,'-'))
	{
		NEXT;
		evalExpr1(theList);
		m_currByteCode->minus();
	}
	else if (getTk(theList,';'))
	{
		return;
	}
}

void ScriptAST::evalExpr01(std::vector<ScriptToken> & theList)
{
	evalExpr02(theList);
	auto tk = GET_TK;
	if (tk.m_str == "==")
	{
		NEXT;
		evalExpr01(theList);
		m_currByteCode->equal();
	}
	else if (tk.m_str == "!=")
	{
		NEXT;
		evalExpr01(theList);
		m_currByteCode->notEqual();
	}
	else if (getTk(theList,';'))
	{
		return;
	}
}

void ScriptAST::evalExpr02(std::vector<ScriptToken> & theList)
{
	evalExpr1(theList);
	auto tk = GET_TK;
	if (tk.m_str == ">")
	{
		NEXT;
		evalExpr02(theList);
		m_currByteCode->greater();
	}
	else if (tk.m_str == "<")
	{
		NEXT;
		evalExpr02(theList);
		m_currByteCode->less();
	}
	else if (tk.m_str == ">=")
	{
		NEXT;
		evalExpr02(theList);
		m_currByteCode->greaterOrEqual();
	}
	else if (tk.m_str == "<=")
	{
		NEXT;
		evalExpr02(theList);
		m_currByteCode->lessOrEqual();
	}
	else if (getTk(theList,';'))
	{
		return;
	}
}

void ScriptAST::evalExpr0(std::vector<ScriptToken> &theList)
{
	evalExpr01(theList);
	if (getTk(theList,'='))
	{
		NEXT;
		evalExpr0(theList);
		m_currByteCode->Assign();
	}
	else if (getTk(theList,';'))
	{
		return;
	}
}

void ScriptAST::evalDefine(std::vector<ScriptToken> & theList)
{
	auto symbolTk = GET_TK;
	if(symbolTk.m_type == ScriptToken::Type::Symbol)
	{
		ScriptValue value;
		value.initWithSymbol(symbolTk.m_str);
		m_currByteCode->pushIteral(value);
		m_currByteCode->define();
		evalExpr0(theList);
	}
	else
	{
		printf("bad define\n");
	}
}

void ScriptAST::evalIf(std::vector<ScriptToken> & theList)
{
	if(!getTk(theList,'(')){printf("bad if statement!\n"); exit(0);}
	NEXT;
	evalExpr0(theList);
	NEXT;
	m_currByteCode->JNE(0);
	// eval true block
	evalBlock(theList);
	m_currByteCode->JMP(1);
	m_currByteCode->label(0);
	if(GET_TK.m_str == "else")
	{
		NEXT;
		// eval false block
		evalBlock(theList);
	}
	m_currByteCode->label(1);
}

void ScriptAST::evalWhile(std::vector<ScriptToken> & theList)
{
	if(!getTk(theList,'(')){printf("bad while statement!\n"); exit(0);}
	NEXT;
	m_currByteCode->label(LABEL_WHILE_LOOP);
	evalExpr0(theList);
	NEXT;
	m_currByteCode->JNE(LABEL_WHILE_BREAK);
	evalBlock(theList);
	m_currByteCode->JMP(-LABEL_WHILE_LOOP);
	m_currByteCode->label(LABEL_WHILE_BREAK);
}

void ScriptAST::evalBlock(std::vector<ScriptToken> & theList)
{
	if(GET_TK.m_str != "{") {printf("bad block!!\n"); exit(0);};
	NEXT;
	while(GET_TK.m_str != "}")
	{
		parseExpr(theList, m_currByteCode);
		NEXT;
	}
}

ScriptToken *ScriptAST::getTk(std::vector<ScriptToken> &theList, int op)
{
	if(m_index >= theList.size())
	{
		return nullptr;
	}else
	{
		if(theList[m_index].m_type == ScriptToken::Op && theList[m_index].m_str[0] == op)
		{
			return &theList[m_index];
		}
		return nullptr;
	}
}

} // namespace tzw
