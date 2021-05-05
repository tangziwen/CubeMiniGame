#include "TinaTokenizer.h"
#include <ctype.h>
namespace tzw
{
void TokenInfo::print()
{
	switch (m_tokenType)
	{
		case TokenType::TOKEN_TYPE_NUM:
		{
			printf("<NUM> %s\n", m_tokenValue.c_str());
		}
		break;
		case TokenType::TOKEN_TYPE_IDENTIFIER:
		{
			printf("<IDENTIFIER> %s\n", m_tokenValue.c_str());
		}
		break;
		case TokenType::TOKEN_TYPE_STR:
		{
			printf("<STR> %s\n", m_tokenValue.c_str());
		}
		break;
		case TokenType::TOKEN_TYPE_OP_PLUS:
		{
			printf("<+>\n");
		}
		break;
		case TokenType::TOKEN_TYPE_OP_MINUS:
		{
			printf("<->\n");
		}
		break;
		case TokenType::TOKEN_TYPE_OP_MULTIPLY:
		{
			printf("<*>\n");
		}
		break;
		case TokenType::TOKEN_TYPE_OP_DIVIDE:
		{
			printf("</>\n");
		}
		break;
		case TokenType::TOKEN_TYPE_OP_ASSIGN:
		{
			printf("<=>\n");
		}
		break;
		case TokenType::TOKEN_TYPE_OP_NOT:
		{
			printf("<!>\n");
		}
		break;
		case TokenType::TOKEN_TYPE_OP_EQUAL:
		{
			printf("<==>\n");
		}
		break;
		case TokenType::TOKEN_TYPE_OP_NOT_EQUAL:
		{
			printf("<!=>\n");
		}
		break;
		case TokenType::TOKEN_TYPE_OP_GREATER:
		{
			printf("<>>\n");
		}
		break;
		case TokenType::TOKEN_TYPE_OP_GREATER_OR_EQUAL:
		{
			printf("<>=>\n");
		}
		break;
		case TokenType::TOKEN_TYPE_OP_LESS:
		{
			printf("<<>\n");
		}
		break;
		case TokenType::TOKEN_TYPE_OP_LESS_OR_EQUAL:
		{
			printf("<<=>\n");
		}
		break;
		case TokenType::TOKEN_TYPE_FINISHED:
		{
			printf("<FINISHED>\n");
		}
		break;
		case TokenType::TOKEN_TYPE_NOT_INVALID:
		{
			printf("<INVALID>\n");
		}
		break;
		default:;
	}
}

bool TokenInfo::isLogicCompare()
{
	return m_tokenType == TokenType::TOKEN_TYPE_OP_GREATER || m_tokenType == TokenType::TOKEN_TYPE_OP_LESS
	|| m_tokenType == TokenType::TOKEN_TYPE_OP_GREATER || m_tokenType == TokenType::TOKEN_TYPE_OP_NOT_EQUAL || m_tokenType == TokenType::TOKEN_TYPE_OP_GREATER_OR_EQUAL || m_tokenType == TokenType::TOKEN_TYPE_OP_LESS_OR_EQUAL;
}

void TinaTokenizer::loadStr(std::string str)
{
	strcpy_s(m_buff, sizeof(m_buff), str.c_str());
	m_buffPos = 0;
}

void TinaTokenizer::nextChar()
{
	m_buffPos += 1;
}

char TinaTokenizer::tryNextChar()
{
	return m_buff[m_buffPos + 1];
}

char TinaTokenizer::currChar()
{
	return m_buff[m_buffPos];
}

TokenInfo TinaTokenizer::getNextToken()
{
	TokenInfo result;
	result.m_tokenType = TokenType::TOKEN_TYPE_NOT_INVALID;
	

	//skip blank characters, new line means blank in tina like C.
	while(currChar() == ' ' || currChar() == '\t' || currChar() == '\r' || currChar() == '\n')
	{
		nextChar();
	}
	char tmpStr[1024];//use to store token string value
	memset(tmpStr,'\0', sizeof(tmpStr));//fill with end of str
	if(currChar() == '\0')
	{
		result.m_tokenType = TokenType::TOKEN_TYPE_FINISHED;
	}
	else if(currChar() == ',')
	{
		nextChar();
		result.m_tokenType = TokenType::TOKEN_TYPE_OP_COMMA;
	}
	//is it a string?
	else if(currChar() == '"')
	{
		nextChar();//skip the left-side quote
		for (int i = 0; currChar() != '"'; i++)
		{
			tmpStr[i] = currChar();
			nextChar();
		}
		nextChar();//skip the right-side quote
		
		result.m_tokenType = TokenType::TOKEN_TYPE_STR;
		result.m_tokenValue = tmpStr;
	}
	//is it a identifier
	else if(isalpha(currChar()) || currChar() == '_')
	{

		for (int i = 0; (isalnum(currChar()) || currChar() == '_'); i++)
		{
			tmpStr[i] = currChar();
			nextChar();
		}
		
		result.m_tokenValue = tmpStr;
		if(result.m_tokenValue == "local")
		{
			result.m_tokenType = TokenType::LOCAL;
		}
		else if(result.m_tokenValue == "print")
		{
			result.m_tokenType = TokenType::TOKEN_TYPE_PRINT;
		}
		else
		{
			result.m_tokenType = TokenType::TOKEN_TYPE_IDENTIFIER;
		}
		
	}
	//is it a number?
	else if(isdigit(currChar()))
	{
		for (int i = 0; (isdigit(currChar()) || currChar() == '.'); i++)
		{
			tmpStr[i] = currChar();
			nextChar();
		}
		result.m_tokenType = TokenType::TOKEN_TYPE_NUM;
		result.m_tokenValue = tmpStr;

	}
	else if(currChar() == ';')
	{
		nextChar();
		result.m_tokenType = TokenType::TOKEN_TYPE_SEMICOLON;
	}
	else if(currChar() == '{')
	{
		nextChar();
		result.m_tokenType = TokenType::TOKEN_TYPE_LEFT_BRACE;
	}
	else if(currChar() == '}')
	{
		nextChar();
		result.m_tokenType = TokenType::TOKEN_TYPE_RIGHT_BRACE;
	}
	else if(currChar() == '(')
	{
		nextChar();
		result.m_tokenType = TokenType::TOKEN_TYPE_LEFT_PARENTHESES;
	}
	else if(currChar() == ')')
	{
		nextChar();
		result.m_tokenType = TokenType::TOKEN_TYPE_RIGHT_PARENTHESES;
	}
	else if(currChar() == '[')
	{
		nextChar();
		result.m_tokenType = TokenType::TOKEN_TYPE_LEFT_SQUARE_BRACKET;
	}
	else if(currChar() == ']')
	{
		nextChar();
		result.m_tokenType = TokenType::TOKEN_TYPE_RIGHT_SQUARE_BRACKET;
	}
	else if(currChar() == '+')
	{
		nextChar();
		result.m_tokenType = TokenType::TOKEN_TYPE_OP_PLUS;
	}

	else if(currChar() == '-')
	{
		nextChar();
		result.m_tokenType = TokenType::TOKEN_TYPE_OP_MINUS;
	}

	else if(currChar() == '*')
	{
		nextChar();
		result.m_tokenType = TokenType::TOKEN_TYPE_OP_MULTIPLY;
	}

	else if(currChar() == '/')
	{
		nextChar();
		result.m_tokenType = TokenType::TOKEN_TYPE_OP_DIVIDE;
	}
	else if(currChar() == '=' && tryNextChar() == '=')
	{
		nextChar();
		nextChar();
		result.m_tokenType = TokenType::TOKEN_TYPE_OP_EQUAL;
	}

	else if(currChar() == '!' && tryNextChar() == '=')
	{
		nextChar();
		nextChar();
		result.m_tokenType = TokenType::TOKEN_TYPE_OP_NOT_EQUAL;
	}

	else if(currChar() == '>' && tryNextChar() == '=')
	{
		nextChar();
		nextChar();
		result.m_tokenType = TokenType::TOKEN_TYPE_OP_GREATER_OR_EQUAL;
	}

	else if(currChar() == '<' && tryNextChar() == '=')
	{
		nextChar();
		nextChar();
		result.m_tokenType = TokenType::TOKEN_TYPE_OP_LESS_OR_EQUAL;
	}

	else if(currChar() == '=')
	{
		nextChar();

		result.m_tokenType = TokenType::TOKEN_TYPE_OP_ASSIGN;
	}

	return result;
}

std::vector<TokenInfo> TinaTokenizer::getTokenList()
{
	std::vector<TokenInfo> resultList;
	for(;;)
	{
		TokenInfo info = getNextToken();
		resultList.push_back(info);
		if(info.m_tokenType == TokenType::TOKEN_TYPE_FINISHED || info.m_tokenType == TokenType::TOKEN_TYPE_NOT_INVALID)
		{
			break;
		}
	}
	return resultList;
}
}

