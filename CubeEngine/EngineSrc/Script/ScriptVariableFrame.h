#ifndef TZW_SCRIPTVARIABLEFRAME_H
#define TZW_SCRIPTVARIABLEFRAME_H

#include <map>
#include <string>
#include "ScriptByteCode.h"
namespace tzw {

class ScriptVariableFrame
{
public:
    ScriptVariableFrame();
    ScriptValue lookUp(std::string name);
    ScriptValue setUp(std::string name,ScriptValue & value);
    void declear(std::string name);
private:
    std::map<std::string ,ScriptValue> m_framePool;
};

} // namespace tzw

#endif // TZW_SCRIPTVARIABLEFRAME_H
