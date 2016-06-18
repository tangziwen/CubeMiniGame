#include "ScriptAST.h"
#include <string.h>
namespace tzw {

#define GET_TK (theList[m_index])

#define GET_OP (theList[m_index].m_str[0])

#define NEXT (m_index++)

#define NEXT_VALID ( !(m_index+1 >= theList.size()))
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
    }else
    {
        evalExpr0(theList);
    }

}

void ScriptAST::evalTerm(std::vector<ScriptToken> &theList)
{
    evalFactor(theList);
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

static int fuck(int a, int b)
{
    return a + b - 1;
}

void ScriptAST::evalFactor(std::vector<ScriptToken> &theList)
{
    auto tk = GET_TK;
    int theValue = 0;
    if (tk.isOp('('))//括号装起来的表达式
    {
        NEXT;
        evalExpr1(theList);
        NEXT;
    }
    else if(tk.m_type == ScriptToken::Type::Integer)//数值
    {
        theValue = GET_TK.m_integer;
        m_currByteCode->pushIteral(ScriptValue(theValue));
        NEXT;
    }
    //是一个符号,那么这个符号要么用来进行函数调用,要么是一个变量名,要根据符号解引用到值.
    else if(tk.m_type == ScriptToken::Type::Symbol)
    {
        if(NEXT_VALID && theList[m_index+1].isOp('('))//调用形式
        {
            if(!strcmp(tk.m_str.c_str(),"fuck"))
            {
                NEXT;//左括号
                NEXT;
                ScriptToken argTK = GET_TK;
                std::vector<int> argList;
                while(!argTK.isOp(')'))
                {
                    argTK = GET_TK;
                    if(!argTK.isOp(','))
                    {
    //                    argList.push_back(evalExpr(theList));//参数列表
                    }else
                    {
                        NEXT;
                    }
                }
            }
        }else//变量形式
        {
            ScriptValue value;
            value.initWithSymbol(tk.m_str);
            m_currByteCode->pushIteral(value);
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

void ScriptAST::evalExpr0(std::vector<ScriptToken> &theList)
{
    evalExpr1(theList);
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
