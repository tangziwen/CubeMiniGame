#ifndef TZW_TOKENIZER_H
#define TZW_TOKENIZER_H

#include <string>
#include <vector>
namespace tzw {

struct ScriptToken{

    enum Type{
        Symbol,
        Str,
        Integer,
        Real,
        Boolean,
        Op,
        KeyWord,
		SpecialSymbol,
    };
    ScriptToken();
    ScriptToken(Type theType);
    bool isOp(int opCode);
    bool isKeyWord(std::string key);
    std::string m_str;
    float m_real;
    int m_integer;
    bool m_boolean;
    Type m_type;
};

class Tokenizer
{
public:
    Tokenizer(std::string theStr);
    void parse();
    std::vector<ScriptToken> m_tokenList;
private:
    std::string m_str;
};

} // namespace tzw

#endif // TZW_TOKENIZER_H
