#include "ScriptByteCode.h"

namespace tzw {

ScriptByteCode::ScriptByteCode()
{

}

void ScriptByteCode::pushIteral(ScriptValue theVar)
{
    ByteCodeCMD cmd;
    cmd.m_type = ByteCodeDef::PushIteral;
    cmd.arg1 = theVar;
    m_cmdList.push_back(cmd);
}

void ScriptByteCode::add()
{
    ByteCodeCMD cmd;
    cmd.m_type = ByteCodeDef::Add;
    m_cmdList.push_back(cmd);
}

void ScriptByteCode::minus()
{
    ByteCodeCMD cmd;
    cmd.m_type = ByteCodeDef::Minus;
    m_cmdList.push_back(cmd);
}

void ScriptByteCode::Assign()
{
    ByteCodeCMD cmd;
    cmd.m_type = ByteCodeDef::Assign;
    m_cmdList.push_back(cmd);
}

void ScriptByteCode::multiple()
{
    ByteCodeCMD cmd;
    cmd.m_type = ByteCodeDef::Multiple;
    m_cmdList.push_back(cmd);
}

void ScriptByteCode::divide()
{
    ByteCodeCMD cmd;
    cmd.m_type = ByteCodeDef::Divide;
    m_cmdList.push_back(cmd);
}

void ScriptByteCode::halt()
{
    ByteCodeCMD cmd;
    cmd.m_type = ByteCodeDef::Halt;
    m_cmdList.push_back(cmd);
}

void ScriptByteCode::print()
{
    ByteCodeCMD cmd;
    cmd.m_type = ByteCodeDef::Print;
    m_cmdList.push_back(cmd);
}
} // namespace tzw
