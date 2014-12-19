#include "shaderpoll.h"
ShaderPoll * ShaderPoll::instance = NULL;
ShaderPoll *ShaderPoll::getInstance()
{
    if(instance)
    {
        return instance;
    }else{
        instance = new ShaderPoll;
        return instance;
    }
}

ShaderProgram *ShaderPoll::get(const char *name)
{
    std::map<std::string,ShaderProgram *>::iterator result = this->shader_list.find(std::string(name));
    if(result != this->shader_list.end())
    {
      return   result->second;
    }else
    {
        return NULL;
    }
}

ShaderProgram *ShaderPoll::createShader(const char *shader_name, const char *vs_name, const char *fs_name)
{
    std::map<std::string,ShaderProgram *>::iterator result = this->shader_list.find(std::string(shader_name));
    if(result == this->shader_list.end())
    {
        ShaderProgram * shader = new ShaderProgram(vs_name,fs_name);
        this->shader_list.insert(std::make_pair(std::string(shader_name),shader));
        return shader;
    }else
    {
        return result->second;
    }
}

ShaderPoll::ShaderPoll()
{
    this->createShader ("shadow","./res/shaders/shadow.vs","./res/shaders/shadow.fs");
    this->createShader("default","./res/shaders/basic.vs","./res/shaders/basic.fs");
    this->createShader("sky_box","./res/shaders/sky_box.vs","./res/shaders/sky_box.fs");
    this->createShader("deferred","./res/shaders/deferred.vs","./res/shaders/deferred.fs");
    this->createShader("dir_light_pass","./res/shaders/lightPass.vs","./res/shaders/DirlightPass.fs");
    this->createShader ("point_light_pass","./res/shaders/lightPass.vs","./res/shaders/pointLightPass.fs");
     this->createShader ("spot_light_pass","./res/shaders/lightPass.vs","./res/shaders/spotLightPass.fs");
    this->createShader ("SSAO","./res/shaders/postProcess.vs","./res/shaders/ssao.fs");
}
