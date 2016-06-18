#ifndef TZW_TEXTUREMGR_H
#define TZW_TEXTUREMGR_H
#include "Texture.h"
#include <map>
#include "../Engine/EngineDef.h"
namespace tzw {

class TextureMgr
{
public:
    TZW_SINGLETON_DECL(TextureMgr)
    Texture * getOrCreateTexture(std::string filePath);
    Texture * getOrCreateTexture(std::string  PosXFilename,
                                 std::string  NegXFilename,
                                 std::string  PosYFilename,
                                 std::string  NegYFilename,
                                 std::string  PosZFilename,
                                 std::string  NegZFilename);
private:
    TextureMgr();
    std::map<std::string,Texture *> m_texturePool;
};

} // namespace tzw

#endif // TZW_TEXTUREMGR_H
