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
	m_stackMap.clear();
	m_constMap.clear();
	
	std::vector<ILCmd> cmdList;
	std::vector<std::string> stackVar;
	std::vector<std::string> envVar;
	std::vector<std::string> constVal;
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

		auto localIter = m_stackMap.find(ast_node->m_op.m_tokenValue);
		if(localIter != m_stackMap.end())//local var
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
			float rawVal = atof(ast_node->m_op.m_tokenValue.c_str());
			val.m_data.valF = rawVal;
			val.m_type = TinaValType::Float;
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
	if(ast_node->m_type == TinaASTNodeType::SEQUENCE)
	{
		OperandLocation lastLocation;
		for(TinaASTNode * child : ast_node->m_children)
		{
			lastLocation = evalR(child, program);
		}
		return lastLocation;
	}
	else if(ast_node->m_type == TinaASTNodeType::LOCAL_DECLARE) // add declare
	{
		
		for(int i = 0; i < ast_node->m_children.size(); i++)
		{
			program.stackVar.push_back(ast_node->m_children[i]->m_op.m_tokenValue);
			m_stackMap[ast_node->m_children[i]->m_op.m_tokenValue] = program.stackVar.size() - 1;
		}
		return noUsedLocation;
	}
	else if(ast_node->m_type == TinaASTNodeType::CALL)
	{
		if(ast_node->m_children.size() > 1)
		{
			for(int i = 1; i < ast_node->m_children.size(); i++)
			{
				evalR(ast_node->m_children[i], program);
			}
		}
		int argNum = ast_node->m_children.size() - 1;
		program.cmdList.push_back(ILCmd(ILCommandType::CALL,
			getLeafAddr(ast_node->m_children[0], program), OperandLocation(OperandLocation::locationType::IMEEDIATE, argNum), OperandLocation(OperandLocation::locationType::REGISTER, m_registerIndex - argNum + 1)));
		m_registerIndex -= argNum;
	}
	else if(ast_node->m_type == TinaASTNodeType::PRINT)
	{
		auto locationL = evalR(ast_node->m_children[0], program);
		program.cmdList.push_back(ILCmd(ILCommandType::PRINT,locationL));
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
					//return resultLocation;
					program.cmdList.push_back(ILCmd(ILCommandType::MOVINDIRECT, locationL, locationR));
					return locationL;
				}
				break;
		}
	}
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
}
