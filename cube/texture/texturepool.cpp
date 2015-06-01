#include "texturepool.h"
TexturePool * TexturePool::instance = NULL;
Texture *TexturePool::createOrGetTexture(const char *file_name)
{
    if(!file_name)
        return NULL;

    Texture * texture = NULL;
    std::map<std::string , Texture * >::iterator result = this->texture_list.find(file_name);
    if(result!= this->texture_list.end())
    {
        texture = result->second;
    }else{
        texture = new Texture(file_name);
        if(texture->getTextureID())
        {
            this->texture_list.insert(std::make_pair(std::string(file_name),texture));
        }else
        {
            delete texture;
            texture = NULL;
        }

    }

    return texture;
}

TexturePool* TexturePool::getInstance()
{
    if(instance)
    {
        return instance;
    }else{
        instance = new TexturePool();
        return instance;
    }
}

TexturePool::TexturePool()
{
    Texture * texture = new Texture("./res/texture/dummy.png");
    this->texture_list.insert(std::make_pair("default",texture));
}
