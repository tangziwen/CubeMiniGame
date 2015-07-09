#ifndef SHADERPOLL_H
#define SHADERPOLL_H
#include <stdlib.h>
#include "shader_program.h"
#include <string>
#include <map>
class ShaderPool
{
public:
    static ShaderPool *getInstance();
    ShaderProgram * get(const char * name);
    ShaderProgram * createShader(const char * shader_name ,const char * vs_name,const char * fs_name);
private:
    ShaderPool();
    static ShaderPool * instance;
    std::map<std::string, ShaderProgram * > shader_list;
};

#endif // SHADERPOLL_H
