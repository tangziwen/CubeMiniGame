#pragma once
#include <string>
#include <vector>
namespace  tzw
{
enum class TokenType
{
	TOKEN_TYPE_NUM,
	TOKEN_TYPE_IDENTIFIER,
	TOKEN_TYPE_STR,
	TOKEN_TYPE_LEFT_BRACE,
	TOKEN_TYPE_RIGHT_BRACE,
	TOKEN_TYPE_LEFT_SQUARE_BRACKET,
	TOKEN_TYPE_RIGHT_SQUARE_BRACKET,
	TOKEN_TYPE_LEFT_PARENTHESES,
	TOKEN_TYPE_RIGHT_PARENTHESES,
	TOKEN_TYPE_SEMICOLON,
	TOKEN_TYPE_PRINT,
	TOKEN_TYPE_OP_COMMA,
	TOKEN_TYPE_OP_PLUS,
	TOKEN_TYPE_OP_MINUS,
	TOKEN_TYPE_OP_MULTIPLY,
	TOKEN_TYPE_OP_DIVIDE,
	TOKEN_TYPE_OP_ASSIGN,
	TOKEN_TYPE_OP_NOT,
	TOKEN_TYPE_OP_EQUAL,
	TOKEN_TYPE_OP_NOT_EQUAL,
	TOKEN_TYPE_OP_GREATER,
	TOKEN_TYPE_OP_GREATER_OR_EQUAL,
	TOKEN_TYPE_OP_LESS,
	TOKEN_TYPE_OP_LESS_OR_EQUAL,
	LOCAL,
	TOKEN_TYPE_FINISHED,
	TOKEN_TYPE_NOT_INVALID,
};

struct TokenInfo
{
	void print();
	TokenType m_tokenType = TokenType::TOKEN_TYPE_NOT_INVALID;
	bool isLogicCompare();
	std::string m_tokenValue;
};

class TinaTokenizer
{
public:
	void loadStr(std::string str);
	void nextChar();
	char tryNextChar();
	char currChar();
	TokenInfo getNextToken();
	std::vector<TokenInfo> getTokenList();
private:
	char  m_buff[65535] = {'\0'};
	int m_buffPos = 0;
	
};
}


