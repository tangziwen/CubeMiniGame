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
};


struct ByteCodeCMD{
    ByteCodeDef m_type;
    ScriptValue arg1;
    ScriptValue arg2;
    ScriptValue arg3;
    ScriptValue arg4;
};

class ScriptByteCode
{
public:
    ScriptByteCode();
    std::vector<ByteCodeCMD> m_cmdList;
    void pushIteral(ScriptValue theVar);
    void add();
    void minus();
    void Assign();
    void multiple();
    void divide();
    void halt();
    void print();
};

} // namespace tzw

#endif // TZW_SCRIPTBYTECODE_H
