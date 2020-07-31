#include "ShaderMgr.h"
#include "Utility/log/Log.h"
#include "Engine/Engine.h"
namespace tzw {

ShaderProgram *ShaderMgr::getByPath(uint32_t mutationFlag, std::string vs, std::string fs, const char * tcs, const char * tes)
{
    shaderInfo info;
    info.fs = fs;
    info.vs = vs;
	info.mutationFlag = mutationFlag;
    auto result = m_pool.find(info);
    if(result == m_pool.end())
    {
		tlog("create shader %s %s", vs.c_str(), fs.c_str());
        ShaderProgram * shader = new ShaderProgram(mutationFlag, vs.c_str(),fs.c_str(), tcs, tes);
        m_pool.insert(std::make_pair(info,shader));
        return shader;
    }else
    {
        return result->second;
    }
}

void ShaderMgr::reloadAllShaders()
{
	for(auto i : m_pool)
	{
		i.second->reload();
	}
}

void ShaderMgr::addMacro(std::string macroName, std::string value)
{
	m_macros[macroName] = value;
}

ShaderMgr::ShaderMgr()
{

}

} // namespace tzw


shaderInfo::shaderInfo():mutationFlag(0)
{
}

shaderInfo::shaderInfo(const shaderInfo& p)
{
	*this = p;
}

bool shaderInfo::operator ==(const shaderInfo &info) const
{
    return fs == info.fs && vs == info.vs && mutationFlag == info.mutationFlag;
}

bool shaderInfo::operator <(const shaderInfo &info) const
{
    return (fs + vs) < (info.fs + info.vs);
}
