#ifndef TZW_SCRIPTBYTECODE_H
#define TZW_SCRIPTBYTECODE_H
#include "ScriptValue.h"
#include <vector>
namespace tzw {
enum class ByteCodeDef{
    PushIteral,
    Add,
    Minus,
    Multiple,
    Divide,
    Assign,
    Halt,
    Print,
    DefVar,
	Call,
	Equal,
	NotEqual,
	Greater,
	Less,
	GreaterOrEqual,
	LessOrEqual,
	JNE,
	JE,
	JMP,
	LABEL,
	GetMember,
	LookUpVar,
	Return,
};


struct ByteCodeCMD{
    ByteCodeDef m_type;
    ScriptValue arg1;
    ScriptValue arg2;
    ScriptValue arg3;
    ScriptValue arg4;
	int tag;
};

class ScriptByteCode
{
public:
    ScriptByteCode();
	std::vector<std::string > m_argList;
    std::vector<ByteCodeCMD> m_cmdList;
	ScriptValue m_returnValue;
    void pushIteral(ScriptValue theVar);
	void Call(ScriptValue value, int argCount);
    void add();
    void minus();
    void Assign();
    void multiple();
    void divide();
    void halt();
    void print();
	void equal();
	void notEqual();
	void greater();
	void less();
	void greaterOrEqual();
	void lessOrEqual();
	void JNE(int tag);
	void JE(int tag);
	void JMP(int tag);
	void define();
	void label(int tag);
	void getMember(std::string memberName);
	void lookUp(std::string variableName);
	void Return(bool hasVal = false);
};

} // namespace tzw

#endif // TZW_SCRIPTBYTECODE_H
