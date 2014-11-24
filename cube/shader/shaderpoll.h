#ifndef SHADERPOLL_H
#define SHADERPOLL_H
#include <stdlib.h>
#include "shader_program.h"
#include <string>
#include <map>
class ShaderPoll
{
public:
    static ShaderPoll *getInstance();
    ShaderProgram * get(const char * name);
    ShaderProgram * createShader(const char * shader_name ,const char * vs_name,const char * fs_name);
private:
    ShaderPoll();
    static ShaderPoll * instance;
    std::map<std::string, ShaderProgram * > shader_list;
};

#endif // SHADERPOLL_H
