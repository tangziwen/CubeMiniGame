#include "ShaderPool.h"
#include "utility.h"
#include "external/TUtility/TUtility.h"
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
        tzw::Tlog()<<tzw::TlogBegin<<"compile Shader "<<shader_name;
        ShaderProgram * shader = new ShaderProgram(vs_name,fs_name);
        this->shader_list.insert(std::make_pair(std::string(shader_name),shader));
        tzw::Tlog()<<"Done."<<tzw::TlogEnd;
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
    this->createShader("sky_box_deferred","./res/shaders/sky_box.vs","./res/shaders/sky_box_geometry.fs");
    this->createShader("deferred","./res/shaders/basic_geometry.vs","./res/shaders/basic_geometry.fs");
    this->createShader("dir_light_pass","./res/shaders/surface.vs","./res/shaders/DirlightPass.fs");
    this->createShader ("point_light_pass","./res/shaders/surface.vs","./res/shaders/pointLightPass.fs");
    this->createShader ("spot_light_pass","./res/shaders/surface.vs","./res/shaders/spotLightPass.fs");
    this->createShader ("gaussian_blur","./res/shaders/surface.vs","./res/shaders/gaussian_blur.fs");
    this->createShader ("gaussian_blur_v","./res/shaders/surface.vs","./res/shaders/gaussian_blur_v.fs");
    this->createShader ("pick_bright","./res/shaders/surface.vs","./res/shaders/pickBright.fs");
    this->createShader ("deffered_simple","./res/shaders/surface.vs","./res/shaders/deffered_simple.fs");
    this->createShader ("linear_blur","./res/shaders/surface.vs","./res/shaders/linearBlur.fs");
    this->createShader ("basic_sprite","./res/shaders/basic_sprite.vs","./res/shaders/basic_sprite.fs");
    this->createShader ("deferred_water","./res/shaders/water_geometry.vs","./res/shaders/water_geometry.fs");
    this->createShader ("deferred_water_PG","./res/shaders/waterPG_Gometry.vs","./res/shaders/waterPG_Gometry.fs");
}

