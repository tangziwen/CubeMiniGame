#include "ShaderMgr.h"
#include <QDebug>
namespace tzw {

TZW_SINGLETON_IMPL(ShaderMgr)

ShaderProgram *ShaderMgr::createOrGet(std::string vs, std::string fs)
{
    shaderInfo info;
    info.fs = fs;
    info.vs = vs;
    auto result = m_pool.find(info);
    if(result == m_pool.end())
    {
        ShaderProgram * shader = new ShaderProgram(vs.c_str(),fs.c_str());
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
