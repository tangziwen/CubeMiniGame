#include "ScriptVM.h"
#include "Tokenizer.h"
#include "ScriptAST.h"
namespace tzw {
#define FECTCH(VALUE) (VALUE.isConstantIteral()?VALUE:lookUpSymbol(VALUE.s()))
ScriptVM::ScriptVM()
{
    m_frameStack.push_back(ScriptVariableFrame());
}


void ScriptVM::excute(std::string procedrueName)
{
    auto theCurrentByteCodeList = m_byteCodeList[procedrueName];
    int theIndex = 0;
    bool isAlive = true;
    while(isAlive)
    {
        auto & cmd = theCurrentByteCodeList->m_cmdList[theIndex];
        switch(cmd.m_type)
        {
        case ByteCodeDef::Add:
        {
            ScriptValue varB = m_evalStack.top();
            m_evalStack.pop();
            ScriptValue varA = m_evalStack.top();
            m_evalStack.pop();
            m_evalStack.push(FECTCH(varB) + FECTCH(varA));
        }
            break;
        case ByteCodeDef::Divide:
        {
            ScriptValue varB = m_evalStack.top();
            m_evalStack.pop();
            ScriptValue varA = m_evalStack.top();
            m_evalStack.pop();
            m_evalStack.push(FECTCH(varB) / FECTCH(varA));
        }
            break;
        case ByteCodeDef::Minus:
        {
            ScriptValue varB = m_evalStack.top();
            m_evalStack.pop();
            ScriptValue varA = m_evalStack.top();
            m_evalStack.pop();
            m_evalStack.push(FECTCH(varB) - FECTCH(varA));
        }
            break;
        case ByteCodeDef::Multiple:
        {
            ScriptValue varB = m_evalStack.top();
            m_evalStack.pop();
            ScriptValue varA = m_evalStack.top();
            m_evalStack.pop();
            m_evalStack.push(FECTCH(varB) * FECTCH(varA));
        }
            break;
        case ByteCodeDef::Print:
        {
            ScriptValue theVar = m_evalStack.top();
            m_evalStack.pop();
            theVar.print();
        }
            break;
        case ByteCodeDef::Assign:
        {
            ScriptValue RV = m_evalStack.top();
            m_evalStack.pop();
            ScriptValue LV = m_evalStack.top();
            m_evalStack.pop();
            if(LV.type() == ScriptValue::Type::Symbol)
            {
                auto result = FECTCH(RV);
                setSymbol(LV.s(),result);
                m_evalStack.push(result);
            }else
            {
                printf("only lvalue can be assign");
            }
        }
        break;
        case ByteCodeDef::PushIteral:
            m_evalStack.push(cmd.arg1);
            break;
        case ByteCodeDef::Halt:
            isAlive = false;
            break;
        }
        theIndex++;
    }
}

void ScriptVM::loadFromStr(std::string str, std::string procedrueName)
{
    Tokenizer tk(str);
    tk.parse();
    size_t theIndex = 0;
    ScriptAST ast;
    auto byteCodeList = new ScriptByteCode();
    //分割成以分号为结束的若干语
    while(theIndex < tk.m_tokenList.size())
    {
        ast.m_index = theIndex;
        ast.parseExpr(tk.m_tokenList,byteCodeList);
        byteCodeList->print();
        theIndex = ast.m_index + 1;
    }
    byteCodeList->halt();
    m_byteCodeList[procedrueName] = byteCodeList;
}

ScriptValue ScriptVM::lookUpSymbol(std::string variableName)
{
    return m_frameStack[0].lookUp(variableName);
}

ScriptValue ScriptVM::setSymbol(std::string varialbleName, const ScriptValue &value)
{

}

void ScriptVM::defineGlobal(std::string name, ScriptValue initValue)
{
    m_frameStack[0].declear(name);
    m_frameStack[0].setUp(name, initValue);
}

void ScriptVM::setGlobal(std::string name, ScriptValue newValue)
{
    m_frameStack[0].setUp(name, newValue);
}

} // namespace tzw
