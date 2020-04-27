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

class ShaderMgr : public Singleton<ShaderMgr>
{
public:
    ShaderProgram * getByPath(std::string vs, std::string fs, const char *tcs = nullptr, const char *tes = nullptr);
	void reloadAllShaders();
	void addMacro(std::string macroName, std::string value);
	std::map<std::string,std::string> m_macros;
	ShaderMgr();
private:
    std::map<shaderInfo,ShaderProgram*> m_pool;
	
};

} // namespace tzw

#endif // TZW_SHADERMGR_H
