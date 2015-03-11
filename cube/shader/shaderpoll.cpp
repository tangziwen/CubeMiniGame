#include "shaderpoll.h"
ShaderPool * ShaderPool::instance = NULL;
ShaderPool *ShaderPool::getInstance()
{
    if(instance)
    {
        return instance;
    }else{
        instance = new ShaderPool;
        return instance;
    }
}

ShaderProgram *ShaderPool::get(const char *name)
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

ShaderProgram *ShaderPool::createShader(const char *shader_name, const char *vs_name, const char *fs_name)
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

ShaderPool::ShaderPool()
{
    this->createShader ("shadow","./res/shaders/shadow.vs","./res/shaders/shadow.fs");
    this->createShader("default","./res/shaders/basic.vs","./res/shaders/basic.fs");
    this->createShader("sky_box","./res/shaders/sky_box.vs","./res/shaders/sky_box.fs");
    this->createShader("sky_box_deferred","./res/shaders/sky_box.vs","./res/shaders/sky_box_deferred.fs");
    this->createShader("deferred","./res/shaders/deferred.vs","./res/shaders/deferred.fs");
    this->createShader("dir_light_pass","./res/shaders/lightPass.vs","./res/shaders/DirlightPass.fs");
    this->createShader ("point_light_pass","./res/shaders/lightPass.vs","./res/shaders/pointLightPass.fs");
    this->createShader ("spot_light_pass","./res/shaders/lightPass.vs","./res/shaders/spotLightPass.fs");
    this->createShader ("gaussian_blur","./res/shaders/postProcess.vs","./res/shaders/gaussian_blur.fs");
    this->createShader ("gaussian_blur_v","./res/shaders/postProcess.vs","./res/shaders/gaussian_blur_v.fs");
    this->createShader ("pick_bright","./res/shaders/postProcess.vs","./res/shaders/pickBright.fs");
    this->createShader ("deffered_simple","./res/shaders/postProcess.vs","./res/shaders/deffered_simple.fs");
    this->createShader ("linear_blur","./res/shaders/postProcess.vs","./res/shaders/linearBlur.fs");
    this->createShader ("basic_sprite","./res/shaders/basic_sprite.vs","./res/shaders/basic_sprite.fs");
    this->createShader ("deferred_water","./res/shaders/deferred_water.vs","./res/shaders/deferred_water.fs");
}
