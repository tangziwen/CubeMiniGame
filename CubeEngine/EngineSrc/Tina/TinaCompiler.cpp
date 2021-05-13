#include "TinaCompiler.h"

#include "TinaParser.h"

namespace tzw
{
ILCmd::ILCmd(ILCommandType type):m_type(type)
{
}

ILCmd::ILCmd(ILCommandType type, OperandLocation A):m_type(type),m_A(A)
{
}

ILCmd::ILCmd(ILCommandType type, OperandLocation A, OperandLocation B):m_type(type),m_A(A), m_B(B)
{
}

ILCmd::ILCmd(ILCommandType type, OperandLocation A, OperandLocation B, OperandLocation C):m_type(type),m_A(A), m_B(B), m_C(C)
{
}

TinaProgram TinaCompiler::gen(TinaASTNode* astRootNode)
{
	TinaProgram program;
	m_constMap.clear();
	evalR(astRootNode, program);
	program.cmdList.push_back(ILCmd(ILCommandType::HALT));
	return program;
}





OperandLocation TinaCompiler::getLeafAddr(TinaASTNode* ast_node, TinaProgram & program)
{
	if(ast_node->m_type != TinaASTNodeType::LEAF)//identifier const up-value
	{
		abort();
	}
	int addr = -1;
	OperandLocation::locationType src = OperandLocation::locationType::ENV;
	if(ast_node->m_op.m_tokenType == TokenType::TOKEN_TYPE_IDENTIFIER)
	{

		auto localIter = m_currParsingFunc->m_stackMap.find(ast_node->m_op.m_tokenValue);
		if(localIter != m_currParsingFunc->m_stackMap.end())//local var
		{
			src = OperandLocation::locationType::STACK;
			addr = localIter->second;
		}
		else // env var
		{
			auto iter = m_envSymbolMap.find(ast_node->m_op.m_tokenValue);
			if(iter == m_envSymbolMap.end())
			{
				program.strLiteral.push_back(ast_node->m_op.m_tokenValue);
				addr = program.strLiteral.size() - 1;
				m_envSymbolMap[ast_node->m_op.m_tokenValue] = addr;
			}
			else
			{
				addr = iter->second;
			}
			src = OperandLocation::locationType::ENV;
		}
		return OperandLocation(src, addr);
	}
	else if(ast_node->m_op.m_tokenType == TokenType::TOKEN_TYPE_STR || ast_node->m_op.m_tokenType == TokenType::TOKEN_TYPE_NUM)
	{
		auto iter = m_constMap.find(ast_node->m_op.m_tokenValue);
		if(iter == m_constMap.end())
		{
			TinaVal val;
			if(ast_node->m_op.m_tokenType == TokenType::TOKEN_TYPE_NUM)
			{
				float rawVal = atof(ast_node->m_op.m_tokenValue.c_str());
				val.m_data.valF = rawVal;
				val.m_type = TinaValType::Float;

			}
			else
			{
				auto strSize = ast_node->m_op.m_tokenValue.size() + 1;
				char * buff = (char*)malloc(strSize);
				strcpy_s(buff,strSize, ast_node->m_op.m_tokenValue.c_str());
				val.m_data.valStr = buff;
				val.m_type = TinaValType::String;
			}

			program.constVal.push_back(val);
			addr = program.constVal.size() - 1;
			m_constMap[ast_node->m_op.m_tokenValue] = addr;
		}
		else
		{
			addr = iter->second;
		}
		src = OperandLocation::locationType::CONSTVAL;
		return OperandLocation(src, addr);
	}

	abort();
}

//#pragma optimize("", off)
//Eval R vaule for expr£¬ it will generate a temp var
OperandLocation TinaCompiler::evalR(TinaASTNode* ast_node, TinaProgram& program)
{
	OperandLocation noUsedLocation;

	if(ast_node->m_type == TinaASTNodeType::FUNC_DEF)
	{
		OperandLocation lastLocation;
		size_t functionJmpAddr = program.cmdList.size();
		//fetchinfo
		TinaFunctionInfo *info = new TinaFunctionInfo();
		info->m_rtInfo.m_entryAddr =functionJmpAddr;
		
		strcpy(info->m_rtInfo.m_name, ast_node->m_op.m_tokenValue.c_str());
		program.functionInfoList.push_back(info);

		//register a env var
		TinaVal * functionObj = new TinaVal();
		functionObj->m_data.valFunctPtr = info->m_rtInfo;
		functionObj->m_type = TinaValType::FuncPtr;
		program.m_envMap[ast_node->m_op.m_tokenValue.c_str()] = functionObj;

		m_currParsingFunc = info;
		
		//parse function parameter List
		for(int i = 0; i < ast_node->m_children[0]->m_children.size(); i++)
		{
			m_currParsingFunc->stackVar.push_back(ast_node->m_children[0]->m_children[i]->m_op.m_tokenValue);
			m_currParsingFunc->m_stackMap[ast_node->m_children[0]->m_children[i]->m_op.m_tokenValue] = m_currParsingFunc->stackVar.size() - 1;
		}
		//generate function body
		lastLocation = evalR(ast_node->m_children[1], program);

		//always generate ret cmd
		program.cmdList.push_back(ILCmd(ILCommandType::RET));
		info->m_rtInfo.m_varCount = info->stackVar.size();
		return lastLocation;
	}
	else if(ast_node->m_type == TinaASTNodeType::FUNC_PARAMETER_LIST)
	{
		OperandLocation lastLocation;
		for(TinaASTNode * child : ast_node->m_children)
		{
			lastLocation = evalR(child, program);
		}
		return lastLocation;
	}
	else if(ast_node->m_type == TinaASTNodeType::SEQUENCE)
	{
		OperandLocation lastLocation;
		for(TinaASTNode * child : ast_node->m_children)
		{
			lastLocation = evalR(child, program);
		}
		return lastLocation;
	}
	else if(ast_node->m_type == TinaASTNodeType::IF)
	{
		OperandLocation noUsedLocation;

		auto testLocation = evalR(ast_node->m_children[0], program);
		program.cmdList.push_back(ILCmd(ILCommandType::JNE, testLocation, OperandLocation(OperandLocation::locationType::IMEEDIATE, 0)));
		int jneJmpCmdIndex = program.cmdList.size() - 1;
		auto if_body = evalR(ast_node->m_children[1], program);
		int elseAddr = program.cmdList.size() + 1;
		program.cmdList[jneJmpCmdIndex].m_B.m_addr = elseAddr;
		program.cmdList.push_back(ILCmd(ILCommandType::JMP, OperandLocation(OperandLocation::locationType::IMEEDIATE, 0)));//jump outside.
		int jmpCmdIndex = program.cmdList.size() - 1;
		if(ast_node->m_children.size() > 2)
		{
			auto else_body = evalR(ast_node->m_children[2], program);
		}
		int outterAddr = program.cmdList.size() + 1;
		program.cmdList[jmpCmdIndex].m_A.m_addr = outterAddr;
		return noUsedLocation;
	}
	else if(ast_node->m_type == TinaASTNodeType::LOCAL_DECLARE) // add declare
	{
		
		for(int i = 0; i < ast_node->m_children.size(); i++)
		{
			m_currParsingFunc->stackVar.push_back(ast_node->m_children[i]->m_op.m_tokenValue);
			m_currParsingFunc->m_stackMap[ast_node->m_children[i]->m_op.m_tokenValue] = m_currParsingFunc->stackVar.size() - 1;
		}
		return noUsedLocation;
	}
	else if(ast_node->m_type == TinaASTNodeType::CALL)
	{
		int argNum = 0;
		if(ast_node->m_children.size() > 1)
		{
			argNum = ast_node->m_children[1]->m_children.size();
		}
		//push stack base pointer.
		program.cmdList.push_back(ILCmd(ILCommandType::PUSH, OperandLocation(OperandLocation::locationType::IMEEDIATE, argNum)));
		
		//apply argument eval argument move to register then move to stack var
		if(ast_node->m_children.size() > 1)
		{
			auto & argList = ast_node->m_children[1]->m_children;
			for(int i = 0; i < argList.size(); i++)
			{
				auto argtmpLoc = evalR(argList[i], program);
				//move to stack
				program.cmdList.push_back(ILCmd(ILCommandType::MOV,
					OperandLocation(OperandLocation::locationType::STACK, i), argtmpLoc));
			}
			decreaseRegIndex(argList.size());
		}
		auto callLocation = evalR(ast_node->m_children[0], program);
		program.cmdList.push_back(ILCmd(ILCommandType::CALL, callLocation, OperandLocation(OperandLocation::locationType::IMEEDIATE, m_currParsingFunc->stackVar.size())));
		decreaseRegIndex(1);
		OperandLocation returnLocation(OperandLocation::locationType::RETREG, 0);
		OperandLocation tmploc = genTmpValue();
		//move return value to a tempory register.
		program.cmdList.push_back(ILCmd(ILCommandType::MOV, tmploc, returnLocation));
		return tmploc;
	}
	else if(ast_node->m_type == TinaASTNodeType::PRINT)
	{
		auto locationL = evalR(ast_node->m_children[0], program);
		program.cmdList.push_back(ILCmd(ILCommandType::PRINT,locationL));
		return noUsedLocation;
	}
	else if(ast_node->m_type == TinaASTNodeType::RETURN)
	{
		//move return value to retRegister.
		auto locationL = evalR(ast_node->m_children[0], program);
		OperandLocation returnLocation(OperandLocation::locationType::RETREG, 0);
		
		//move return value to return value register
		decreaseRegIndex(1);
		program.cmdList.push_back(ILCmd(ILCommandType::MOV, returnLocation, locationL));
		//pop stack base pointer.
		program.cmdList.push_back(ILCmd(ILCommandType::POP, OperandLocation(OperandLocation::locationType::IMEEDIATE, m_currParsingFunc->m_rtInfo.m_varCount)));
		//add ret command.
		program.cmdList.push_back(ILCmd(ILCommandType::RET));

		return noUsedLocation;
	}
	else if(ast_node->m_type == TinaASTNodeType::LEAF)//identifier const up-value
	{
		auto resultLocation = genTmpValue();
		program.cmdList.push_back(ILCmd(ILCommandType::MOV, resultLocation, getLeafAddr(ast_node, program)));
		return resultLocation;
	}
	else if(ast_node->m_type == TinaASTNodeType::OPERATOR)
	{
		switch (ast_node->m_op.m_tokenType)
		{
			case TokenType::TOKEN_TYPE_OP_PLUS:
				{
					auto locationL = evalR(ast_node->m_children[0], program);
					auto locationR = evalR(ast_node->m_children[1], program);
					decreaseRegIndex(2);
					auto resultLocation = genTmpValue();
					program.cmdList.push_back(ILCmd(ILCommandType::ADD, 
						resultLocation, locationL, locationR));
					return resultLocation;
				}
				break;
			case TokenType::TOKEN_TYPE_OP_MINUS:
				{
					auto locationL = evalR(ast_node->m_children[0], program);
					auto locationR = evalR(ast_node->m_children[1], program);
					decreaseRegIndex(2);
					auto resultLocation = genTmpValue();
					
					program.cmdList.push_back(ILCmd(ILCommandType::SUB, 
						resultLocation, locationL, locationR));
					return resultLocation;
				}
				break;
			case TokenType::TOKEN_TYPE_OP_MULTIPLY:
				{
					auto locationL = evalR(ast_node->m_children[0], program);
					auto locationR = evalR(ast_node->m_children[1], program);
					decreaseRegIndex(2);
					auto resultLocation = genTmpValue();
					program.cmdList.push_back(ILCmd(ILCommandType::MUL, 
						resultLocation, locationL, locationR));
					return resultLocation;
				}
				break;
			case TokenType::TOKEN_TYPE_OP_DIVIDE:
				{
					auto locationL = evalR(ast_node->m_children[0], program);
					auto locationR = evalR(ast_node->m_children[1], program);
					decreaseRegIndex(2);
					auto resultLocation = genTmpValue();
					program.cmdList.push_back(ILCmd(ILCommandType::DIV, 
						resultLocation, locationL, locationR));
					return resultLocation;
				}
				break;
			case TokenType::TOKEN_TYPE_OP_ASSIGN://!!!! L-vaule use it own addr
				{
					
					auto locationR = evalR(ast_node->m_children[1], program);
					auto locationL = evalL(ast_node->m_children[0], program);
					decreaseRegIndex(2);
					program.cmdList.push_back(ILCmd(ILCommandType::MOVINDIRECT, locationL, locationR));
					return locationL;
				}
				break;
			case TokenType::TOKEN_TYPE_OP_GREATER:
				{
					auto locationL = evalR(ast_node->m_children[0], program);
					auto locationR = evalR(ast_node->m_children[1], program);
					decreaseRegIndex(2);
					auto resultLocation = genTmpValue();
					program.cmdList.push_back(ILCmd(ILCommandType::GRT, 
						resultLocation, locationL, locationR));
					return resultLocation;
				}
				break;
		}
	}
	return noUsedLocation;
}

OperandLocation TinaCompiler::evalL(TinaASTNode* ast_node, TinaProgram& program)
{
	if(ast_node->m_type == TinaASTNodeType::LEAF)//identifier const up-value
	{
		auto tmpAddr = genTmpValue();
		//move the address to register.
		program.cmdList.push_back(ILCmd(ILCommandType::LEA, tmpAddr, getLeafAddr(ast_node, program)));
		return tmpAddr;
	}
	abort();
}

OperandLocation TinaCompiler::genTmpValue()
{
	auto tmpAddr = OperandLocation(OperandLocation::locationType::REGISTER, m_registerIndex);
	m_registerIndex++;
	return tmpAddr;
}

void TinaCompiler::decreaseRegIndex(int count)
{
	m_registerIndex -= count;
}
TinaFunctionInfo* TinaProgram::findFunctionInfoFromName(std::string funcName)
{
	for(int i = 0; i < functionInfoList.size(); i++)
	{
		if(funcName == functionInfoList[i]->m_rtInfo.m_name)
		{
			return functionInfoList[i];
		}
	}
	return nullptr;
}
}
