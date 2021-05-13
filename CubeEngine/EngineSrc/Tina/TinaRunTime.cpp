#include "TinaRunTime.h"

#include <cassert>

namespace tzw
{
void TinaRunTime::execute(TinaProgram* program, std::string functionName)
{
	//find function entry addr
	auto info = program->findFunctionInfoFromName(functionName);
	//copy global definition
	m_envMap = program->m_envMap;
	if(info)
	{
		m_PC = info->m_rtInfo.m_entryAddr;
		m_SBP = 0;
		m_SP = info->m_rtInfo.m_varCount + m_SBP;
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
				else if (cmd.m_A.m_locSrc == OperandLocation::locationType::STACK)
				{
					m_stack[m_SBP + cmd.m_A.m_addr] = val;
				}
				else if (cmd.m_A.m_locSrc == OperandLocation::locationType::RETREG)
				{
					m_retReg = val;
				}
			}
			break;
			case ILCommandType::PUSH://push stack base pointer
			{
				m_SBP = m_SP;
				m_SP += cmd.m_A.m_addr;
			}
			break;
			case ILCommandType::POP:
			{
				m_SP = m_SBP;
				m_SBP -= m_funcAddrStack.top().localVarCount;
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
			case ILCommandType::LOG_GRT:
			{
				TinaVal a, b;
				getVal(program, cmd.m_B, &a);
				getVal(program, cmd.m_C, &b);

				//To
				if(cmd.m_A.m_locSrc == OperandLocation::locationType::REGISTER)//To ref in specified register.
				{
					m_register[cmd.m_A.m_addr] = valGreater(&a, &b);
				}
					
			}break;
			case ILCommandType::LOG_GE:
			{
				TinaVal a, b;
				getVal(program, cmd.m_B, &a);
				getVal(program, cmd.m_C, &b);

				//To
				if(cmd.m_A.m_locSrc == OperandLocation::locationType::REGISTER)//To ref in specified register.
				{
					m_register[cmd.m_A.m_addr] = valGreaterOREqual(&a, &b);
				}
					
			}break;
			case ILCommandType::LOG_LES:
			{
				TinaVal a, b;
				getVal(program, cmd.m_B, &a);
				getVal(program, cmd.m_C, &b);

				//To
				if(cmd.m_A.m_locSrc == OperandLocation::locationType::REGISTER)//To ref in specified register.
				{
					m_register[cmd.m_A.m_addr] = valLess(&a, &b);
				}
					
			}break;
			case ILCommandType::LOG_LE:
			{
				TinaVal a, b;
				getVal(program, cmd.m_B, &a);
				getVal(program, cmd.m_C, &b);

				//To
				if(cmd.m_A.m_locSrc == OperandLocation::locationType::REGISTER)//To ref in specified register.
				{
					m_register[cmd.m_A.m_addr] = valLessOrEqual(&a, &b);
				}
					
			}break;
			case ILCommandType::LOG_EQ:
			{
				TinaVal a, b;
				getVal(program, cmd.m_B, &a);
				getVal(program, cmd.m_C, &b);

				//To
				if(cmd.m_A.m_locSrc == OperandLocation::locationType::REGISTER)//To ref in specified register.
				{
					m_register[cmd.m_A.m_addr] = valEqual(&a, &b);
				}
					
			}break;
			case ILCommandType::LOG_NE:
			{
				TinaVal a, b;
				getVal(program, cmd.m_B, &a);
				getVal(program, cmd.m_C, &b);

				//To
				if(cmd.m_A.m_locSrc == OperandLocation::locationType::REGISTER)//To ref in specified register.
				{
					m_register[cmd.m_A.m_addr] = valNotEqual(&a, &b);
				}
					
			}break;
			case ILCommandType::JMP:
			{
				m_PC = cmd.m_A.m_addr;
				continue;
			}break;
			case ILCommandType::JE:
			{
				TinaVal conditionVal;
				getVal(program, cmd.m_A, &conditionVal);
				if(conditionVal.m_data.valI)
				{
					m_PC = cmd.m_B.m_addr;
					continue;
				}
			}
			break;
			case ILCommandType::JNE:
			{
				TinaVal conditionVal;
				getVal(program, cmd.m_A, &conditionVal);
				if(!conditionVal.m_data.valI)
				{
					m_PC = cmd.m_B.m_addr;
					continue;
				}
			}
			break;
			case ILCommandType::CALL:
			{
				printf("call\n");
				//store the PC & current layer function stack var count
				FunctionEntryInfo entryInfo;
				entryInfo.pc = m_PC;
				entryInfo.localVarCount = cmd.m_B.m_addr;
				m_funcAddrStack.push(entryInfo);
					
				TinaVal a;
				getVal(program, cmd.m_A, &a);
				//jump to function
				auto& info = a.m_data.valFunctPtr;

				m_PC = info.m_entryAddr;
				continue;

			}break;
			case ILCommandType::RET:
			{
				if(m_funcAddrStack.empty())
				{
					return;//halft
				}
				else
				{
					m_PC = m_funcAddrStack.top().pc;//restore the PC
					//restore the SBP
					//m_SBP += cmd.m_A.m_addr;
					m_funcAddrStack.pop();
				}
			}break;
			case ILCommandType::PRINT:
			{
				TinaVal val;
				getVal(program, cmd.m_A, &val);
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
	else if(location.m_locSrc == OperandLocation::locationType::RETREG)//To ref in special return register.
	{
		*val = m_retReg;
	}
	else if(location.m_locSrc == OperandLocation::locationType::STACK)//To ref in specified register.
	{
		*val = m_stack[m_SBP + location.m_addr];
	}
}

}
