#include "ShaderMgr.h"
#include "Base/Log.h"
namespace tzw {

TZW_SINGLETON_IMPL(ShaderMgr)

ShaderProgram *ShaderMgr::getByPath(std::string vs, std::string fs, const char * tcs, const char * tes)
{
    shaderInfo info;
    info.fs = fs;
    info.vs = vs;
    auto result = m_pool.find(info);
    if(result == m_pool.end())
    {
		tlog("create shader %s %s\n", vs.c_str(), fs.c_str());
        ShaderProgram * shader = new ShaderProgram(vs.c_str(),fs.c_str(), tcs, tes);
		tlog("after create shader\n");
        m_pool.insert(std::make_pair(info,shader));
        return shader;
    }else
    {
        return result->second;
    }
}

ShaderMgr::ShaderMgr()
{

}

} // namespace tzw



bool shaderInfo::operator ==(const shaderInfo &info) const
{
    return fs == info.fs && vs == info.vs;
}

bool shaderInfo::operator <(const shaderInfo &info) const
{
    return fs < info.fs;
}
