#ifndef MATERIALPOOL_H
#define MATERIALPOOL_H
#include "material.h"
class MaterialPool
{
public:
    Material *createMaterial(const char * the_name);
    static MaterialPool *getInstance();
private:
   static  MaterialPool * instance;
    MaterialPool();
    std::map<std::string , Material * > texture_list;
};

#endif // MATERIALPOOL_H
