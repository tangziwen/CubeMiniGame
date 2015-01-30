#ifndef CUBEPRIMITVE_H
#define CUBEPRIMITVE_H
#include "entity.h"
//cube primitive
class CubePrimitve : public Entity
{
public:
    CubePrimitve(const char * textureFile);
    ~CubePrimitve();
};

#endif // CUBEPRIMITVE_H
