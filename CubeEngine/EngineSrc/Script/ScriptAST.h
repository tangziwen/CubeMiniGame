#ifndef TZW_SCRIPTAST_H
#define TZW_SCRIPTAST_H

#include "Tokenizer.h"
#include "ScriptByteCode.h"
namespace tzw {
class ScriptVM;
class ScriptAST
{
public:
    ScriptAST();
    void parseExpr(std::vector<ScriptToken> &theList, ScriptByteCode * byteCode);
    size_t m_index;
	ScriptVM * m_vm;
private:
    void evalTerm(std::vector<ScriptToken> &theList);
	void evalSubTerm(std::vector<ScriptToken> &theList);
    void evalFactor(std::vector<ScriptToken> &theList);
    void evalExpr1(std::vector<ScriptToken> &theList);
	void evalExpr01(std::vector<ScriptToken> &theList);
	void evalExpr02(std::vector<ScriptToken> &theList);
    void evalExpr0(std::vector<ScriptToken> &theList);
	void evalDefine(std::vector<ScriptToken> &theList);
	void evalIf(std::vector<ScriptToken> &theList);
	void evalWhile(std::vector<ScriptToken> &theList);
	void evalBlock(std::vector<ScriptToken> & theList);
    ScriptByteCode * m_currByteCode;
    ScriptToken * getTk(std::vector<ScriptToken> &theList,int op);
};

} // namespace tzw

#endif // TZW_SCRIPTAST_H
