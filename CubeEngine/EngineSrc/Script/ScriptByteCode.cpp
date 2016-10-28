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

void ScriptByteCode::Call(ScriptValue value, int argCount)
{
	ByteCodeCMD cmd;
    cmd.m_type = ByteCodeDef::Call;
    cmd.arg1 = value;
	cmd.arg2.setI(argCount);
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

void ScriptByteCode::equal()
{
	ByteCodeCMD cmd;
    cmd.m_type = ByteCodeDef::Equal;
	m_cmdList.push_back(cmd);
}

void ScriptByteCode::notEqual()
{
	ByteCodeCMD cmd;
    cmd.m_type = ByteCodeDef::NotEqual;
	m_cmdList.push_back(cmd);
}

void ScriptByteCode::greater()
{
	ByteCodeCMD cmd;
    cmd.m_type = ByteCodeDef::Greater;
	m_cmdList.push_back(cmd);
}

void ScriptByteCode::less()
{
	ByteCodeCMD cmd;
    cmd.m_type = ByteCodeDef::Less;
	m_cmdList.push_back(cmd);
}

void ScriptByteCode::greaterOrEqual()
{
	ByteCodeCMD cmd;
    cmd.m_type = ByteCodeDef::GreaterOrEqual;
	m_cmdList.push_back(cmd);
}

void ScriptByteCode::lessOrEqual()
{
	ByteCodeCMD cmd;
    cmd.m_type = ByteCodeDef::LessOrEqual;
	m_cmdList.push_back(cmd);
}

void ScriptByteCode::JNE(int tag)
{
	ByteCodeCMD cmd;
    cmd.m_type = ByteCodeDef::JNE;
	cmd.tag = tag;
	m_cmdList.push_back(cmd);
}

void ScriptByteCode::JE(int tag)
{
	ByteCodeCMD cmd;
    cmd.m_type = ByteCodeDef::JE;
	cmd.tag = tag;
	m_cmdList.push_back(cmd);
}

void ScriptByteCode::JMP(int tag)
{
	ByteCodeCMD cmd;
	cmd.m_type = ByteCodeDef::JMP;
	cmd.tag = tag;
	m_cmdList.push_back(cmd);

}

void ScriptByteCode::define()
{
	ByteCodeCMD cmd;
    cmd.m_type = ByteCodeDef::DefVar;
	m_cmdList.push_back(cmd);
}

void ScriptByteCode::label(int tag)
{
	ByteCodeCMD cmd;
    cmd.m_type = ByteCodeDef::LABEL;
	cmd.tag = tag;
	m_cmdList.push_back(cmd);
}

void ScriptByteCode::getMember(std::string memberName)
{
	ByteCodeCMD cmd;
    cmd.m_type = ByteCodeDef::GetMember;
	cmd.arg1.setS(memberName);
	m_cmdList.push_back(cmd);
}

void ScriptByteCode::lookUp(std::string variableName)
{
	ByteCodeCMD cmd;
	cmd.m_type = ByteCodeDef::LookUpVar;
	cmd.arg1.setS(variableName);
	m_cmdList.push_back(cmd);
}

void ScriptByteCode::Return(bool hasVal)
{
	ByteCodeCMD cmd;
	cmd.m_type = ByteCodeDef::LookUpVar;
	cmd.arg1 = ScriptValue(hasVal);
	m_cmdList.push_back(cmd);
}

} // namespace tzw
