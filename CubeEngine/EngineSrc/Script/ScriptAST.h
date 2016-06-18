#ifndef TZW_SCRIPTAST_H
#define TZW_SCRIPTAST_H

#include "Tokenizer.h"
#include "ScriptByteCode.h"
namespace tzw {

class ScriptAST
{
public:
    ScriptAST();
    void parseExpr(std::vector<ScriptToken> &theList, ScriptByteCode * byteCode);
    size_t m_index;
private:
    void evalTerm(std::vector<ScriptToken> &theList);
    void evalFactor(std::vector<ScriptToken> &theList);
    void evalExpr1(std::vector<ScriptToken> &theList);
    void evalExpr0(std::vector<ScriptToken> &theList);
    ScriptByteCode * m_currByteCode;
    ScriptToken * getTk(std::vector<ScriptToken> &theList,int op);
};

} // namespace tzw

#endif // TZW_SCRIPTAST_H
