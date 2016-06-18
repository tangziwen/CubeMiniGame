#include "TextureMgr.h"
#include <utility>
namespace tzw {

TZW_SINGLETON_IMPL(TextureMgr)

Texture *TextureMgr::getOrCreateTexture(std::string filePath)
{
    auto result = m_texturePool.find(filePath);
    if(result!=m_texturePool.end())
    {
        return result->second;
    }else
    {
        Texture * tex = new Texture(filePath);
        m_texturePool.insert(std::make_pair(filePath,tex));
        return tex;
    }
}

Texture *TextureMgr::getOrCreateTexture(std::string PosXFilename, std::string NegXFilename, std::string PosYFilename, std::string NegYFilename, std::string PosZFilename, std::string NegZFilename)
{
    auto result = m_texturePool.find(PosXFilename);
    if(result!=m_texturePool.end())
    {
        return result->second;
    }else
    {
        Texture * tex = new Texture(PosXFilename,NegXFilename,PosYFilename,NegYFilename,PosZFilename,NegZFilename);
        m_texturePool.insert(std::make_pair(PosXFilename,tex));
        return tex;
    }
}

TextureMgr::TextureMgr()
{

}

} // namespace tzw

