#ifndef TEXTUREPOOL_H
#define TEXTUREPOOL_H
#include "texture.h"
#include <map>
#include <string>
class TexturePool
{
public:
    //get a texture if it's not exist ,create one
    Texture *createOrGetTexture(const char * file_name);
    static TexturePool *getInstance();
private:
   static  TexturePool * instance;
    TexturePool();
    std::map<std::string , Texture * > texture_list;
};

#endif // TEXTUREPOOL_H
