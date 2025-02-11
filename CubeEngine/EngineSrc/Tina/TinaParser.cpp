#include "TinaParser.h"

namespace tzw
{
TinaASTNode::TinaASTNode(TokenInfo op,TinaASTNodeType type):m_op(op),m_parent(nullptr),m_type(type)
{
}

TinaASTNode::TinaASTNode(TinaASTNodeType type):m_parent(nullptr),m_type(type)
{
}

void TinaASTNode::addChild(TinaASTNode* node)
{
	m_children.push_back(node);
	node->m_parent = this;
}

void TinaParser::parse(std::vector<TokenInfo> tokenList)
{
	m_tokenList = tokenList;
	m_tokenPos = 0;
	m_root = new TinaASTNode(TinaASTNodeType::SEQUENCE);
	while(currToken().m_tokenType != TokenType::TOKEN_TYPE_FINISHED)
	{
		switch(currToken().m_tokenType)
		{
		case TokenType::TOKEN_TYPE_FUNCDEF:
		{
			auto funcDefNode = parseFunctionDef();
			m_root->addChild(funcDefNode);
		}
		break;
		case TokenType::TOKEN_TYPE_LEFT_BRACE:
		{
			auto exprNode = parseStatement();
			m_root->addChild(exprNode);
		}
		break;
		
		}
	}
}

TinaASTNode* TinaParser::getRoot()
{
	return m_root;
}

TinaASTNode* TinaParser::parseFunctionDef()
{
	auto funcNode = new TinaASTNode(TinaASTNodeType::FUNC_DEF);
	nextToken();
	funcNode->m_op = currToken();//functionName
	//follow a parameter list
	auto paramNode = new TinaASTNode(TinaASTNodeType::FUNC_PARAMETER_LIST);
	funcNode->addChild(paramNode);
	nextToken();//eat the left (
	while(currToken().m_tokenType != TokenType::TOKEN_TYPE_RIGHT_PARENTHESES)
	{
		//ignore comma
		if(currToken().m_tokenType == TokenType::TOKEN_TYPE_IDENTIFIER)
		{
			paramNode->addChild(new TinaASTNode(currToken(), TinaASTNodeType::LEAF));
		}
		nextToken();
	}
	nextToken();//eat the right )
	funcNode->addChild(parseStatement());//add the function body
	return funcNode;
}

TinaASTNode* TinaParser::parseStatement()
{
	TinaASTNode * node;
	{
		node = parseFlowControlStatement();
	}
	return node;
}

TinaASTNode * TinaParser::parseFlowControlStatement()
{
	auto blockNode = new TinaASTNode(TinaASTNodeType::SEQUENCE);
	TinaASTNode * statementNode = nullptr;
	if(currToken().m_tokenType == TokenType::TOKEN_TYPE_IF || currToken().m_tokenType == TokenType::TOKEN_TYPE_WHILE)
	{
		if(currToken().m_tokenType == TokenType::TOKEN_TYPE_IF)//start with If
		{
			statementNode = new TinaASTNode(TinaASTNodeType::IF);
			nextToken();//eat the if

			nextToken();//eat the left (
			statementNode->addChild(parseExpr());
			nextToken();//eat the right )
			statementNode->addChild(parseFlowControlStatement());//parse if Block

			if(currToken().m_tokenType == TokenType::TOKEN_TYPE_ELSE)
			{
				nextToken();//eat the else
				statementNode->addChild(parseFlowControlStatement());//parse else Block
			}

		}
		else if(currToken().m_tokenType == TokenType::TOKEN_TYPE_WHILE)//start with while
		{
			statementNode = new TinaASTNode(TinaASTNodeType::WHILE);
			nextToken();//eat the while

			nextToken();//eat the left (
			statementNode->addChild(parseExpr());
			nextToken();//eat the right )
			statementNode->addChild(parseBlockStatement());//parse loop body Block
		}
		blockNode->addChild(statementNode);
	}
	else
	{
		auto node = parseBlockStatement();
		blockNode->addChild(node);
	}
	return blockNode;
}

TinaASTNode* TinaParser::parseBlockStatement()
{
	auto blockNode = new TinaASTNode(TinaASTNodeType::SEQUENCE);

	if(currToken().m_tokenType == TokenType::TOKEN_TYPE_LEFT_BRACE)
	{
		nextToken();//eat the left brace
		while(currToken().m_tokenType != TokenType::TOKEN_TYPE_RIGHT_BRACE)
		{
			TinaASTNode * statementNode = parseStatement();
			blockNode->addChild(statementNode);
		}
		nextToken();//eat the right brace
	}
	else
	{
		if(currToken().isCmdToken())
		{
			TinaASTNode * node = parsePrintStatement();
			blockNode->addChild(node);
		}
		else if(currToken().m_tokenType == TokenType::LOCAL )
		{
			TinaASTNode * node = parseLocalDeclare();
			blockNode->addChild(node);
		}
		else
		{
			TinaASTNode * statementNode = parseSingleStatement();
			blockNode->addChild(statementNode);
		}

	}
	return blockNode;
}

TinaASTNode* TinaParser::parsePrintStatement()
{
	TinaASTNode * statementNode = nullptr;
	if(currToken().m_tokenType == TokenType::TOKEN_TYPE_PRINT)//start with print
	{
		statementNode = new TinaASTNode(TinaASTNodeType::PRINT);
		nextToken();//eat the print
		statementNode->addChild(parseSingleStatement());
	}
	else if(currToken().m_tokenType == TokenType::TOKEN_TYPE_RETURN)//start with return
	{
		statementNode = new TinaASTNode(TinaASTNodeType::RETURN);
		nextToken();//eat the return
		statementNode->addChild(parseSingleStatement());
	}
	else
	{
		statementNode = parseSingleStatement();
	}
	return statementNode;
}


TinaASTNode* TinaParser::parseSingleStatement()
{
	TinaASTNode * statementNode = nullptr;
	if(currToken().m_tokenType == TokenType::TOKEN_TYPE_SEMICOLON)//empty statement
	{
		statementNode = new TinaASTNode(TinaASTNodeType::SEQUENCE);
		nextToken();
	}
	else if(currToken().m_tokenType == TokenType::TOKEN_TYPE_BREAK)//break statement
	{
		statementNode = new TinaASTNode(TinaASTNodeType::BREAK);
		nextToken();//eat the break;
		nextToken();//eat the semicolon;
	}
	else if(currToken().m_tokenType == TokenType::TOKEN_TYPE_CONTINUE)//continue statement
	{
		statementNode = new TinaASTNode(TinaASTNodeType::CONTINUE);
		nextToken();//eat the continue;
		nextToken();//eat the semicolon;
	}
	else if(currToken().m_tokenType == TokenType::TOKEN_TYPE_SEMICOLON)//empty statement
	{
		statementNode = new TinaASTNode(TinaASTNodeType::SEQUENCE);
		nextToken();
	}
	else
	{
		statementNode = parseExpr();
		nextToken();//eat the semicolon
	}
	return statementNode;
}


TinaASTNode* TinaParser::parseLocalDeclare()
{
	TinaASTNode * localDeclare = nullptr;
	if(currToken().m_tokenType == TokenType::LOCAL)//local a {, b, c, d} ;
	{
		localDeclare = new TinaASTNode(TinaASTNodeType::LOCAL_DECLARE);
		nextToken();
		auto child = new TinaASTNode(currToken(), TinaASTNodeType::LEAF);
		localDeclare->addChild(child);
		nextToken();
		if(currToken().m_tokenType == TokenType::TOKEN_TYPE_OP_COMMA)
		{
			while(currToken().m_tokenType == TokenType::TOKEN_TYPE_OP_COMMA)
			{
				nextToken();//eat comma
				child = new TinaASTNode(currToken(), TinaASTNodeType::LEAF);
				localDeclare->addChild(child);
			}
			nextToken();//eat ;
		}
		else if(currToken().m_tokenType == TokenType::TOKEN_TYPE_SEMICOLON)
		{
			nextToken();//eat ;
		}
	}
	else
	{
		localDeclare = parseSingleStatement();
	}
	return localDeclare;
}

TinaASTNode* TinaParser::parseExpr()
{
	TinaASTNode * leftNode = parseAssign();
	if(currToken().m_tokenType == TokenType::TOKEN_TYPE_OP_COMMA)
	{
		TinaASTNode * opNode = nullptr;
		while(currToken().m_tokenType == TokenType::TOKEN_TYPE_OP_COMMA)
		{
			opNode = new TinaASTNode(currToken(), TinaASTNodeType::OPERATOR);
			nextToken();
			auto rightNode = parseAssign();
			opNode->addChild(leftNode);
			opNode->addChild(rightNode);

			leftNode = opNode;
		}
		return opNode;
	}
	return leftNode;
}

TinaASTNode* TinaParser::parseAssign()
{
	TinaASTNode * leftNode;

	leftNode = parseCompExpr();

	if(currToken().m_tokenType == TokenType::TOKEN_TYPE_OP_ASSIGN)
	{
		//lvaule check
		if(leftNode->m_type != TinaASTNodeType::LEAF || 
			((leftNode->m_type == TinaASTNodeType::OPERATOR) && currToken().m_tokenType != TokenType::TOKEN_TYPE_LEFT_SQUARE_BRACKET))
		{
			abort();//can't not produce lvaule
		}
		leftNode->m_isLvalue = true;

		
		TinaASTNode * opNode = nullptr;
		opNode = new TinaASTNode(currToken(), TinaASTNodeType::OPERATOR);
		nextToken();
		auto rightNode = parseAssign(); //right recursion for right asscoicaty
		opNode->addChild(leftNode);
		opNode->addChild(rightNode);
		return opNode;
		
	}
	return leftNode;
}

TinaASTNode* TinaParser::parseCompExpr()
{
	TinaASTNode * left = parseArithExpr();
	if(currToken().isLogicCompare())
	{
		TinaASTNode * op = new TinaASTNode(currToken(), TinaASTNodeType::OPERATOR);
		nextToken();
		TinaASTNode * right = parseArithExpr();
		op->addChild(left);
		op->addChild(right);
		return op;
	}
	else
	{
		return left;
	}
}

TinaASTNode * TinaParser::parseArithExpr()
{
	auto leftNode = parseTerm();

	if(currToken().m_tokenType == TokenType::TOKEN_TYPE_OP_PLUS || currToken().m_tokenType == TokenType::TOKEN_TYPE_OP_MINUS)
	{
		TinaASTNode * opNode = nullptr;
		while(currToken().m_tokenType == TokenType::TOKEN_TYPE_OP_PLUS || currToken().m_tokenType == TokenType::TOKEN_TYPE_OP_MINUS)
		{
			opNode = new TinaASTNode(currToken(), TinaASTNodeType::OPERATOR);
			nextToken();
			auto rightNode = parseTerm();
			opNode->addChild(leftNode);
			opNode->addChild(rightNode);

			leftNode = opNode;
		}
		return opNode;
	}
	else
	{
		
		return leftNode;
	}
}

TinaASTNode * TinaParser::parseTerm()
{
	
	auto leftNode = parsePostfixExpr();
	if(currToken().m_tokenType == TokenType::TOKEN_TYPE_OP_MULTIPLY || currToken().m_tokenType == TokenType::TOKEN_TYPE_OP_DIVIDE)
	{
		TinaASTNode * opNode = nullptr;
		while (currToken().m_tokenType == TokenType::TOKEN_TYPE_OP_MULTIPLY || currToken().m_tokenType == TokenType::TOKEN_TYPE_OP_DIVIDE)
		{
			opNode = new TinaASTNode(currToken(), TinaASTNodeType::OPERATOR);
			
			nextToken();
			auto rightNode = parsePostfixExpr();
			opNode->addChild(leftNode);
			opNode->addChild(rightNode);

			leftNode = opNode;
		}
		return opNode;
	}
	else
	{
		return leftNode;
	}

}

TinaASTNode* TinaParser::parsePostfixExpr()
{
	auto leftNode = parseFactor();
	//function call a(xxx,xxx)
	if(currToken().m_tokenType == TokenType::TOKEN_TYPE_LEFT_PARENTHESES)
	{
		TinaASTNode * opNode = nullptr;
		//invoke like a(void)
		if(tryNextToken().m_tokenType == TokenType::TOKEN_TYPE_RIGHT_PARENTHESES)
		{
			nextToken();
			nextToken();//skip right
			opNode = new TinaASTNode(TinaASTNodeType::CALL);
			opNode->addChild(leftNode);
			
		}
		else
		{
			//invoke like a(xxx) || a(xxx,xxx...) || a(xxx,xxx...)(xxx,xxx...)...
			while(currToken().m_tokenType == TokenType::TOKEN_TYPE_LEFT_PARENTHESES)
			{
				opNode = new TinaASTNode(TinaASTNodeType::CALL);

				nextToken();
				auto rightNode = parseExpr();
				nextToken();//skip right parentheses
				
				opNode->addChild(leftNode);
				opNode->addChild(rightNode);
				
				leftNode = opNode;
			}
		}
		return opNode;
	}
	//array index refrence a[xx] || a[xx][xx]...
	else if(currToken().m_tokenType == TokenType::TOKEN_TYPE_LEFT_SQUARE_BRACKET)
	{
		TinaASTNode * opNode = nullptr;
		while(currToken().m_tokenType == TokenType::TOKEN_TYPE_LEFT_SQUARE_BRACKET)
		{
			opNode = new TinaASTNode(currToken(), TinaASTNodeType::OPERATOR);
			nextToken();
			auto rightNode = parseExpr();
			nextToken();//skip right square bracket
			
			opNode->addChild(leftNode);
			opNode->addChild(rightNode);
			
			leftNode = opNode;
		}
		return opNode;
	}
	else
	{
		return leftNode;
	}
}

TinaASTNode * TinaParser::parseFactor()
{
	TinaASTNode * node = nullptr;
	if(currToken().m_tokenType == TokenType::TOKEN_TYPE_IDENTIFIER)
	{
		node = new TinaASTNode(currToken(), TinaASTNodeType::LEAF);
		nextToken();
	}
	else if(currToken().m_tokenType == TokenType::TOKEN_TYPE_STR)
	{
		node = new TinaASTNode(currToken(), TinaASTNodeType::LEAF);
		nextToken();
	}
	else if(currToken().m_tokenType == TokenType::TOKEN_TYPE_NUM)
	{
		node = new TinaASTNode(currToken(), TinaASTNodeType::LEAF);
		nextToken();
	}
	else if(currToken().m_tokenType == TokenType::TOKEN_TYPE_LEFT_PARENTHESES)
	{
		nextToken();
		node = parseArithExpr();
		nextToken();
	}
	
	return node;
}

TokenInfo& TinaParser::currToken()
{
	return m_tokenList[m_tokenPos];
}

TokenInfo& TinaParser::nextToken()
{
	if(m_tokenPos < m_tokenList.size() -1 )
	{
		m_tokenPos ++;
	}
	return m_tokenList[m_tokenPos];
}

TokenInfo& TinaParser::tryNextToken()
{
	return m_tokenList[m_tokenPos + 1];
}
}
