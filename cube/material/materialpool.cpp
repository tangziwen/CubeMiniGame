#include "materialpool.h"
#include "texture/texturepool.h"
MaterialPool * MaterialPool::instance = NULL;
Material *MaterialPool::createOrGetMaterial(const char *the_name)
{
    Material * material = NULL;
    std::map<std::string , Material * >::iterator result = this->texture_list.find(the_name);
    if(result!= this->texture_list.end())
    {
        material = result->second;
    }else{
        material = new Material();
        this->texture_list.insert(std::make_pair(std::string(the_name),material));
    }
    return material;
}

MaterialPool *MaterialPool::getInstance()
{
    if(instance)
    {
        return instance;
    }else{
        instance = new MaterialPool();
        return instance;
    }
}

MaterialPool::MaterialPool()
{
    Material * material = new Material();
    material->getDiffuse()->texture = TexturePool::getInstance()->createOrGetTexture("default");
    this->texture_list.insert(std::make_pair(std::string("default"),material));
}
