#include "ScriptVM.h"

#include "ScriptAST.h"
#include "ScripStdLib.h"
#include <iostream>
#include "External/TUtility/TUtility.h"
#include "ScriptStruct.h"
namespace tzw {
ScriptVM::ScriptVM()
{
	m_funcID = 0;
	m_frameStack.push_back(ScriptVariableFrame());
}


void ScriptVM::excute(unsigned int procedrueName)
{
	auto theCurrentByteCodeList = m_byteCodeList[procedrueName];
	int theIndex = 0;
	bool isAlive = true;
	while(isAlive && theIndex < theCurrentByteCodeList->m_cmdList.size())
	{
		auto & cmd = theCurrentByteCodeList->m_cmdList[theIndex];
		switch(cmd.m_type)
		{
			case ByteCodeDef::Add:
				{
					ScriptValue valB = m_evalStack.top();
					m_evalStack.pop();
					ScriptValue valA = m_evalStack.top();
					m_evalStack.pop();
					m_evalStack.push(valA.toRV() + valB.toRV());
				}
			break;
			case ByteCodeDef::GetMember:
				{
					ScriptValue valA = m_evalStack.top();
					m_evalStack.pop();
					if(valA.toRV().type() != ScriptValue::Type::structPtr)
					{
						printf("bad index, the LV is not locate to a Struct \n");
						exit(0);
					}
					auto thestrcut = valA.toRV().getStructPtr();
					m_evalStack.push(ScriptValue(thestrcut->get(cmd.arg1.s())));
				}
			break;
			case ByteCodeDef::Divide:
				{
					ScriptValue valB = m_evalStack.top();
					m_evalStack.pop();
					ScriptValue valA = m_evalStack.top();
					m_evalStack.pop();
					m_evalStack.push(valA.toRV() / valB.toRV());
				}
			break;
			case ByteCodeDef::Minus:
				{
					ScriptValue valB = m_evalStack.top();
					m_evalStack.pop();
					ScriptValue valA = m_evalStack.top();
					m_evalStack.pop();
					m_evalStack.push(valA.toRV() - valB.toRV());
				}
			break;
			case ByteCodeDef::Multiple:
				{
					ScriptValue valB = m_evalStack.top();
					m_evalStack.pop();
					ScriptValue valA = m_evalStack.top();
					m_evalStack.pop();
					m_evalStack.push(valA.toRV() * valB.toRV());
				}
			break;
			case ByteCodeDef::Print:
				{
					if(!m_evalStack.empty())
					{
						ScriptValue theVar = m_evalStack.top();
						m_evalStack.pop();
						Tlog() << theVar.toStr();
					}
				}
			break;
			case ByteCodeDef::Assign:
				{
					ScriptValue RV = m_evalStack.top();
					m_evalStack.pop();
					ScriptValue LV = m_evalStack.top();
					m_evalStack.pop();
					if(LV.type() == ScriptValue::Type::LValue)
					{
						*LV.getRefValue() = RV.toRV();
					}
					else
					{
						printf("only lvalue can be assign\n");
						exit(0);
					}
				}
			break;
			case ByteCodeDef::Call:
				{
					std::string funcName = cmd.arg1.s();
					int argCount = cmd.arg2.i();
					auto func = lookUpLocal(funcName).toRV();
					std::vector<ScriptValue> argValList;
					for(int i = argCount; i > 0; i --)
					{
						auto value = m_evalStack.top();
						m_evalStack.pop();
						argValList.push_back(value.toRV());
					}
					switch(func.type())
					{
						//user defined function
						case ScriptValue::Type::UserFunction:
							{
								for(auto iter : argValList)
								{
									m_userStack.push(iter);
								}
								m_userStack.push(ScriptValue(argCount));
								func.getUserFunction()(this);
								if(!m_userStack.empty())
								{
									m_evalStack.push(m_userStack.top());
								}
								userStackClear();
							}
						break;
						case ScriptValue::Type::ScriptFunc:
							{
								auto tmpStack = m_evalStack;
								incFrame();
								int i = 0;
								if(getByteCode(func.i())->m_argList.size() != argValList.size())
								{
									printf("parameter is not match!!!!\n");
									exit(0);
								}
								for(auto iter: getByteCode(func.i())->m_argList)
								{
									setLocal(iter, argValList[i]);
									i++;
								}
								excute(func.i());
								decFrame();
								m_evalStack = tmpStack;
								if(getByteCode(func.i())->m_returnValue.type() != ScriptValue::Type::Impossible)
								{
									m_evalStack.push(getByteCode(func.i())->m_returnValue);
								}
							}
						break;
						default:
							printf("invalid function call %s\n", funcName.c_str());
							exit(0);
						break;
					}
				}
			break;
			case ByteCodeDef::Return:
				{
					if(cmd.arg1.b())
					{
						theCurrentByteCodeList->m_returnValue = m_evalStack.top();
						m_evalStack.pop();
					}
					theCurrentByteCodeList->m_returnValue = ScriptValue(ScriptValue::Type::Impossible);
					isAlive = false;
				}
			break;
			case ByteCodeDef::DefVar:
				{
					ScriptValue LV = m_evalStack.top();
					m_evalStack.pop();
					if(LV.type() == ScriptValue::Type::Symbol)
					{
						defineLocal(LV.s());
					}else
					{
						printf("only Symbol can be def");
					}
				}
			break;
			case ByteCodeDef::Equal:
				{
					ScriptValue A = m_evalStack.top();
					m_evalStack.pop();
					ScriptValue B = m_evalStack.top();
					m_evalStack.pop();
					m_evalStack.push(A.toRV() == B);
				}
			break;
			case ByteCodeDef::NotEqual:
				{
					ScriptValue A = m_evalStack.top();
					m_evalStack.pop();
					ScriptValue B = m_evalStack.top();
					m_evalStack.pop();
					m_evalStack.push(A.toRV() != B.toRV());
				}
			break;
			case ByteCodeDef::Greater:
				{
					ScriptValue A = m_evalStack.top();
					m_evalStack.pop();
					ScriptValue B = m_evalStack.top();
					m_evalStack.pop();
					m_evalStack.push(B.toRV() > A.toRV());
				}
			break;
			case ByteCodeDef::Less:
				{
					ScriptValue A = m_evalStack.top();
					m_evalStack.pop();
					ScriptValue B = m_evalStack.top();
					m_evalStack.pop();
					m_evalStack.push(B.toRV() < A.toRV());
				}
			break;
			case ByteCodeDef::GreaterOrEqual:
				{
					ScriptValue A = m_evalStack.top();
					m_evalStack.pop();
					ScriptValue B = m_evalStack.top();
					m_evalStack.pop();
					m_evalStack.push(B.toRV() >= A.toRV());
				}
			break;
			case ByteCodeDef::LookUpVar:
				{
					auto val = this->lookUpLocal(cmd.arg1.s());
					if(val.type() != ScriptValue::Type::Impossible)
					{
						m_evalStack.push(val);
					}else
					{
						m_evalStack.push(defineLocal(cmd.arg1.s()));
					}
				}
			break;
			case ByteCodeDef::JNE:
				{
					ScriptValue A = m_evalStack.top();
					m_evalStack.pop();
					if(!A.toRV().b())
					{
						while(true)
						{
							auto tmpCmd = theCurrentByteCodeList->m_cmdList[theIndex];
							if(tmpCmd.m_type == ByteCodeDef::LABEL && tmpCmd.tag == cmd.tag)
							{
								break;
							}
							theIndex ++;
						}
					}
				}
			break;
			case ByteCodeDef::JMP:
				{
					int step = cmd.tag >= 0 ? 1 : -1;
					int tag = abs(cmd.tag);
					while(true)
					{
						auto tmpCmd = theCurrentByteCodeList->m_cmdList[theIndex];
						if(tmpCmd.m_type == ByteCodeDef::LABEL && tmpCmd.tag == tag)
						{
							break;
						}
						theIndex += step;
					}
				}
			break;
			case ByteCodeDef::LABEL:
				{
					// do nothing.
				}
			break;
			case ByteCodeDef::LessOrEqual:
				{
					ScriptValue A = m_evalStack.top();
					m_evalStack.pop();
					ScriptValue B = m_evalStack.top();
					m_evalStack.pop();
					m_evalStack.push(B.toRV() <= A.toRV());
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

void ScriptVM::loadFromStr(std::string str, unsigned int procedrueName)
{
	Tokenizer tk(str);
	tk.parse();
	size_t theIndex = 0;
	ScriptAST ast;
	ast.m_vm = this;
	auto byteCodeList = new ScriptByteCode();
	while(theIndex < tk.m_tokenList.size())
	{
		ast.m_index = theIndex;
		ast.parseExpr(tk.m_tokenList,byteCodeList);
		theIndex = ast.m_index + 1;
	}
	if(m_byteCodeList.find(procedrueName) != m_byteCodeList.end())
	{
		auto oldList = m_byteCodeList[procedrueName];
		delete oldList;
	}
	m_byteCodeList[procedrueName] = byteCodeList;
}

int ScriptVM::evalFromTKList(std::vector<ScriptToken> tkList, int index, unsigned int procedrueName)
{
	ScriptAST ast;
	ast.m_vm = this;
	size_t i = index;
	auto byteCodeList = new ScriptByteCode();
	ast.m_index = i;
	ast.parseExpr(tkList,byteCodeList);
	if(m_byteCodeList.find(procedrueName) != m_byteCodeList.end())
	{
		auto oldList = m_byteCodeList[procedrueName];
		delete oldList;
	}
	m_byteCodeList[procedrueName] = byteCodeList;
	return ast.m_index;
}

void ScriptVM::loadFromFile(std::string fileName, unsigned int procedrueName)
{
	auto data = Tfile::shared()->getData(fileName, true);
	loadFromStr(data.getString(), procedrueName);
}

ScriptValue ScriptVM::lookUpLocal(std::string variableName)
{
	for(auto iter = m_frameStack.rbegin(); iter != m_frameStack.rend(); iter++)
	{
		auto value = iter->lookUp(variableName);
		if(value.type() != ScriptValue::Type::Impossible)
		{
			return value;
		}
	}
	return ScriptValue(ScriptValue::Type::Impossible);
}

ScriptValue ScriptVM::setLocal(std::string varialbleName, const ScriptValue &value)
{
	for(auto iter = m_frameStack.rbegin(); iter != m_frameStack.rend(); iter++)
	{
		auto result = iter->lookUp(varialbleName);
		if(result.type() != ScriptValue::Type::Impossible)
		{
			iter->setUp(varialbleName, value);
			return value;
		}
	}
	//找不到就在最大栈帧设置
	m_frameStack.rbegin()->setUp(varialbleName, value);
	return value;
}

ScriptValue ScriptVM::defineLocal(std::string variableName)
{
	const auto& frame = m_frameStack.rbegin();
	return frame->define(variableName);
}

ScriptValue ScriptVM::lookUpGlobal(std::string variableName)
{
	auto value = m_frameStack[0].lookUp(variableName);
	if(value.type() != ScriptValue::Type::Impossible)
	{
		return value;
	}
	return ScriptValue(ScriptValue::Type::Impossible);
}

void ScriptVM::defineGlobal(std::string variableName)
{
	m_frameStack[0].define(variableName);
}

void ScriptVM::setGlobal(std::string name, ScriptValue newValue)
{
	m_frameStack[0].setUp(name, newValue);
}

ScriptValue ScriptVM::userStackPop()
{
	if(!m_userStack.empty())
	{
		auto value = m_userStack.top();
		m_userStack.pop();
		return value;
	}else
	{
		return ScriptValue(ScriptValue::Type::Impossible);
	}
}

void ScriptVM::userStackPush(ScriptValue value)
{
	m_userStack.push(value);
}

void ScriptVM::userStackClear()
{
	while(!m_userStack.empty())
	{
		m_userStack.pop();
	}
}

void ScriptVM::defineFunction(std::string funcName, std::function<void (ScriptVM *)> func)
{
	defineGlobal(funcName);
	setGlobal(funcName, ScriptValue(func));
}

void ScriptVM::useStdLibrary()
{
	ScripStdLib::init(this);
}

void ScriptVM::addHeapObj(ScriptStruct * theStruct)
{
	m_heap.push_back(theStruct);
}

unsigned int ScriptVM::getAvailableFuncID()
{
	return (++m_funcID);
}

void ScriptVM::incFrame()
{
	m_frameStack.push_back(ScriptVariableFrame());
}

void ScriptVM::decFrame()
{
	m_frameStack.pop_back();
}

ScriptByteCode *ScriptVM::getByteCode(unsigned int funcID)
{
	return m_byteCodeList[funcID];
}

} // namespace tzw
