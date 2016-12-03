#include "Tokenizer.h"
#include <ctype.h>
#include <algorithm>
#include <string.h>
namespace tzw {

const char * keyWordList[] = {"if","else","var","goto","true","false", "{", "}","while", "break", "function", "return"};
static bool isOp(char c)
{
    return (c =='+' || c =='-' || c =='*' || c =='/' || c=='(' || c==')' || c==',' || c==';' || c == '=' || c =='>' || c =='<' || c =='!'
			|| c == '.');
}

static bool findKeyWorld(std::string theStr)
{
	size_t keywordCount = sizeof(keyWordList) / sizeof(char *);
	for(size_t i = 0; i < keywordCount; i++)
	{
		if(!strcmp(keyWordList[i],theStr.c_str()))
			return true;
	}
	return false;
}

Tokenizer::Tokenizer(std::string theStr)
{
    m_str = theStr;
}

void Tokenizer::parse()
{
    const char * buf = m_str.c_str();
    int theIndex = 0;
    while(buf[theIndex])
    {
        //Skip Empty and space.
        if(isspace(buf[theIndex]))
        {
            theIndex ++;
            while(isspace(buf[theIndex]))
            {
                theIndex++;
            }
        }
        //操作符
        if(isOp(buf[theIndex]))
        {
           ScriptToken tk(ScriptToken::Type::Op);
           tk.m_str ="";
		   if((buf[theIndex] == '=' || buf[theIndex] == '>' || buf[theIndex] == '<' || buf[theIndex] == '!') && buf[theIndex + 1] == '=')
		   {
			   tk.m_str.append(1,buf[theIndex]);
			   tk.m_str.append(1,buf[theIndex + 1]);
			   theIndex += 2;
		   }else
		   {
			   tk.m_str.append(1,buf[theIndex]);
			   theIndex++;
		   }

           m_tokenList.push_back(tk);
        }
        //整数或浮点数
        if(isdigit(buf[theIndex]))
        {
            ScriptToken tk;
            bool isReal = false;
            std::string digitStr = "";
            digitStr.append(1, buf[theIndex]);
            theIndex++;
            while(isdigit(buf[theIndex]) || buf[theIndex]=='.')
            {
                if(buf[theIndex]== '.')
                {
                    isReal = true;
                }
                digitStr.append(1, buf[theIndex]);
                theIndex++;
            }
            tk.m_str = digitStr;
            if(isReal)
            {
                tk.m_type = ScriptToken::Type::Real;
                tk.m_real = static_cast<float>(atof(digitStr.c_str()));
            }else
            {
                tk.m_type = ScriptToken::Type::Integer;
                tk.m_integer = atoi(digitStr.c_str());
            }
            m_tokenList.push_back(tk);
        }
        //字符串
        if(buf[theIndex] =='\"')
        {
            ScriptToken tk(ScriptToken::Type::Str);
            theIndex ++;
            std::string tmpStr = "";
            while(buf[theIndex]!='\"')
            {
                tmpStr.append(1,buf[theIndex]);
                theIndex ++;
            }
            theIndex++;//跳过右引号
            tk.m_str = tmpStr;
            m_tokenList.push_back(tk);
        }

        //符号表
        if(isalpha(buf[theIndex]))
        {
            ScriptToken tk(ScriptToken::Type::Symbol);
            std::string tmpStr = "";
            while(isalnum(buf[theIndex]))
            {
                tmpStr.append(1,buf[theIndex]);
                theIndex ++;
            }
            tk.m_str = tmpStr;
			if(findKeyWorld(tmpStr))
			{
				//一些例外
				if (tmpStr == "true" || tmpStr == "false")
				{
					tk.m_type = ScriptToken::Type::Boolean;
					tk.m_boolean = (tmpStr == "true") ? true : false;
				}else
				{
					tk.m_type = ScriptToken::Type::KeyWord;
				}
			}
            m_tokenList.push_back(tk);
        }
		if(buf[theIndex] == '{' || buf[theIndex] == '}')
		{
			ScriptToken tk(ScriptToken::Type::SpecialSymbol);
			tk.m_str.append(1,buf[theIndex]);
			theIndex ++;
			m_tokenList.push_back(tk);
		}
    }
}

ScriptToken::ScriptToken()
{

}

ScriptToken::ScriptToken(ScriptToken::Type theType)
{
    m_type = theType;
}

bool ScriptToken::isOp(int opCode)
{
    return(opCode == m_str[0] && m_type == Type::Op);
}

bool ScriptToken::isKeyWord(std::string key)
{
    return key == m_str && m_type == Type::KeyWord;
}

} // namespace tzw
