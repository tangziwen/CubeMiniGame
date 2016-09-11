#ifndef TZW_SHADERMGR_H
#define TZW_SHADERMGR_H
#include <map>
#include <string>
#include "ShaderProgram.h"
#include "../Engine/EngineDef.h"

struct shaderInfo
{
    std::string fs;
    std::string vs;
    bool operator ==(const  shaderInfo& info)const;
    bool operator <(const shaderInfo &info)const;
};

namespace tzw {

class ShaderMgr
{
public:
    TZW_SINGLETON_DECL(ShaderMgr)
    ShaderProgram * createOrGet(std::string vs, std::string fs, const char *tcs = nullptr, const char *tes = nullptr);
private:
    ShaderMgr();
    std::map<shaderInfo,ShaderProgram*> m_pool;
};

} // namespace tzw

#endif // TZW_SHADERMGR_H
