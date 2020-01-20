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
    Texture * getByPath(std::string filePath, bool isNeedMipMap = false);
	Texture * getByPathSimple(std::string filePath);
	Texture* getByPathAsync(std::string filePath, std::function<void (Texture *)> finishedCallBack, bool isNeedMiMap = false);
	Texture* loadAsync(std::string filePath, std::function<void (Texture *)> finishedCallBack, bool isNeedMiMap = false);
    Texture * getByPath(std::string  PosX,
                                 std::string  NegX,
                                 std::string  PosY,
                                 std::string  NegY,
                                 std::string  PosZ,
                                 std::string  NegZ);
	Texture * loadSingleCubeMap(std::string filePath);
private:
    TextureMgr();
    std::map<std::string,Texture *> m_texturePool;
};

} // namespace tzw

#endif // TZW_TEXTUREMGR_H
