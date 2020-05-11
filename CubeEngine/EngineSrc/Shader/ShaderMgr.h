#ifndef TZW_SHADERMGR_H
#define TZW_SHADERMGR_H
#include <map>
#include <string>
#include "ShaderProgram.h"
#include "../Engine/EngineDef.h"




struct shaderInfo
{
	shaderInfo();
	shaderInfo(const shaderInfo& p);
    std::string fs;
    std::string vs;
	uint32_t mutationFlag;
    bool operator ==(const  shaderInfo& info)const;
    bool operator <(const shaderInfo &info)const;
};
struct KeyHasher
{
  std::size_t operator()(const shaderInfo& k) const
  {
    using std::size_t;
    using std::hash;
    using std::string;

    return ((hash<string>()(k.fs)
             ^ (hash<string>()(k.vs) << 1)) >> 1)
             ^ (hash<uint32_t>()(k.mutationFlag) << 1);
  }
};
namespace tzw {

class ShaderMgr : public Singleton<ShaderMgr>
{
public:
    ShaderProgram * getByPath(uint32_t mutationFlag, std::string vs, std::string fs, const char *tcs = nullptr, const char *tes = nullptr);
	void reloadAllShaders();
	void addMacro(std::string macroName, std::string value);
	std::map<std::string,std::string> m_macros;
	ShaderMgr();
private:
    std::unordered_map<shaderInfo,ShaderProgram*, KeyHasher> m_pool;
	
};

} // namespace tzw

#endif // TZW_SHADERMGR_H
