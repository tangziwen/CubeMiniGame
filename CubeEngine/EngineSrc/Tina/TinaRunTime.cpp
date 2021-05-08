#include "TinaRunTime.h"

#include <cassert>

namespace tzw
{
void TinaRunTime::execute(TinaProgram* program, std::string functionName)
{
	//find function entry addr
	auto info = program->findFunctionInfoFromName(functionName);
	if(info)
	{
		m_PC = info->m_entryAddr;
	}

	for (;;)
	{
		auto& cmd = program->cmdList[m_PC];
		if (cmd.m_type == ILCommandType::HALT)
		{
			break;
		}
		switch (cmd.m_type)
		{
			case ILCommandType::MOV:// mov target src
			{
				TinaVal val;
				//From

				getVal(program, cmd.m_B, &val);

				//To
				if(cmd.m_A.m_locSrc == OperandLocation::locationType::REGISTER)//To ref in specified register.
				{
					m_register[cmd.m_A.m_addr] = val;
				}
			}
			break;
			case ILCommandType::PUSH:
			{
			}
			break;
			case ILCommandType::POP:
			{
			}break;
			case ILCommandType::ADD:
			{
				TinaVal a, b;
				getVal(program, cmd.m_B, &a);
				getVal(program, cmd.m_C, &b);

				//To
				if(cmd.m_A.m_locSrc == OperandLocation::locationType::REGISTER)//To ref in specified register.
				{
					m_register[cmd.m_A.m_addr] = valAdd(&a, &b);
				}
					
			}break;
			case ILCommandType::SUB:
			{
				TinaVal a, b;
				getVal(program, cmd.m_B, &a);
				getVal(program, cmd.m_C, &b);

				//To
				if(cmd.m_A.m_locSrc == OperandLocation::locationType::REGISTER)//To ref in specified register.
				{
					m_register[cmd.m_A.m_addr] = valSub(&a, &b);
				}
					
			}break;
			case ILCommandType::MUL:
			{
				TinaVal a, b;
				getVal(program, cmd.m_B, &a);
				getVal(program, cmd.m_C, &b);

				//To
				if(cmd.m_A.m_locSrc == OperandLocation::locationType::REGISTER)//To ref in specified register.
				{
					m_register[cmd.m_A.m_addr] = valMul(&a, &b);
				}
					
			}break;
			case ILCommandType::DIV:
			{
				TinaVal a, b;
				getVal(program, cmd.m_B, &a);
				getVal(program, cmd.m_C, &b);

				//To
				if(cmd.m_A.m_locSrc == OperandLocation::locationType::REGISTER)//To ref in specified register.
				{
					m_register[cmd.m_A.m_addr] = valDiv(&a, &b);
				}
					
			}break;
			case ILCommandType::JMP:
			{
			}break;
			case ILCommandType::CALL:
			{
				printf("call\n");
				//store the PC
				m_funcAddrStack.push(m_PC);
				//jump to function
				auto info = program->findFunctionInfoFromName("func1");
				if(info)
				{
					m_PC = info->m_entryAddr;
					continue;
				}
			}break;
			case ILCommandType::RET:
			{
				if(m_funcAddrStack.empty())
				{
					return;//halft
				}
				else
				{
					m_PC = m_funcAddrStack.top();//restore the PC
					m_funcAddrStack.pop();
				}
			}break;
			case ILCommandType::PRINT:
			{
				TinaVal val;
				//From
				if(cmd.m_A.m_locSrc == OperandLocation::locationType::REGISTER)//from register
				{
					val = m_register[cmd.m_A.m_addr];
				}
				else if (cmd.m_A.m_locSrc == OperandLocation::locationType::CONSTVAL)
				{
					val = program->constVal[cmd.m_A.m_addr];
				}

				printf("[PRNT]%s\n", val.toStr());
			}break;
			case ILCommandType::HALT:
			{
			}break;
			case ILCommandType::MOVINDIRECT: //MOV [target] src
			{
				TinaVal val;
				//From
				if(cmd.m_B.m_locSrc == OperandLocation::locationType::REGISTER)//from register
				{
					val = m_register[cmd.m_B.m_addr];
				}
				else if (cmd.m_B.m_locSrc == OperandLocation::locationType::CONSTVAL)
				{
					val = program->constVal[cmd.m_B.m_addr];
				}

				//To
				if(cmd.m_A.m_locSrc == OperandLocation::locationType::REGISTER)//To ref in specified register.
				{
					TinaVal ref = m_register[cmd.m_A.m_addr];
					assert(ref.m_type == TinaValType::Ref);
					(*ref.m_data.valRef) = val;
				}
			}break;
			case ILCommandType::LEA:// lea
			{
				TinaVal val;
				//From
				if(cmd.m_B.m_locSrc == OperandLocation::locationType::ENV)//from env, we need to store string literal
				{
					val = refFromEnv(program->strLiteral[cmd.m_B.m_addr]);
				}
				//From
				if(cmd.m_B.m_locSrc == OperandLocation::locationType::STACK)//from Stack
				{
					val = refFromStack(cmd.m_B.m_addr);
				}
				//to
				if(cmd.m_A.m_locSrc == OperandLocation::locationType::REGISTER)
				{
					m_register[cmd.m_A.m_addr] = val;
				}
				
			}
			break;
			default:;
		}

		m_PC++;
	}
}

TinaVal TinaRunTime::refFromEnv(std::string identifier)
{
	TinaVal resultVal;
	resultVal.m_type = TinaValType::Ref;
	auto result = m_envMap.find(identifier);
	if(result == m_envMap.end())
	{
		TinaVal * val = new TinaVal();
		m_envMap[identifier] = val;
		resultVal.m_data.valRef = val;
		
	}
	else
	{
		resultVal.m_data.valRef = result->second;
	}

	return resultVal;
}

TinaVal TinaRunTime::refFromStack(int stackID)
{
	
	TinaVal resultVal;
	resultVal.m_type = TinaValType::Ref;

	resultVal.m_data.valRef = &m_stack[m_SBP + stackID];
	return resultVal;
}

void TinaRunTime::getVal(TinaProgram* program, OperandLocation location, TinaVal* val)
{
	//From
	if(location.m_locSrc == OperandLocation::locationType::ENV)//from env
	{
		*val = (*refFromEnv(program->strLiteral[location.m_addr]).m_data.valRef);
	}
	else if (location.m_locSrc == OperandLocation::locationType::CONSTVAL)
	{
		*val = program->constVal[location.m_addr];
	}
	else if(location.m_locSrc == OperandLocation::locationType::REGISTER)//To ref in specified register.
	{
		*val = m_register[location.m_addr];
	}
	else if(location.m_locSrc == OperandLocation::locationType::STACK)//To ref in specified register.
	{
		*val = m_stack[m_SBP + location.m_addr];
	}
}
}
