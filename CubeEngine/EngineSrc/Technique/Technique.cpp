#include "Technique.h"
#include <utility>
#include <stdlib.h>
#include "../Shader/ShaderMgr.h"
#include "../BackEnd/RenderBackEnd.h"
namespace tzw {

/**
 * @brief Technique::Technique 构造函数
 * @param vsFilePath 顶点shader的文件路径
 * @param fsFilePath 片段shader的文件路径
 */
Technique::Technique(const char *vsFilePath, const char *fsFilePath)
    :m_vsPath(vsFilePath),m_fsPath(fsFilePath)
{
    m_program = ShaderMgr::shared()->createOrGet(vsFilePath,fsFilePath);
}

/**
 * @brief Technique::setVar 设置Technique的变量
 * @param name 变量的类型
 * @param value 值
 * @note Technique会自动缓存这些变量，只要Technique还在生存周期内，这些值都会不断的提交到shader
 */
void Technique::setVar(std::string name, const QMatrix4x4 & value)
{
    auto result = m_varList.find(name);
    if(result != m_varList.end())
    {
        TechniqueVar * var =  result->second;
        var->setM(value);
    }else
    {
        TechniqueVar * var = new TechniqueVar();
        var->setM(value);
        m_varList.insert(std::make_pair(name,var));
    }
}

/**
 * @brief Technique::setVar 设置Technique的变量
 * @param name 变量的类型
 * @param value 值
 * @note Technique会自动缓存这些变量，只要Technique还在生存周期内，这些值都会不断的提交到shader
 */
void Technique::setVar(std::string name, const float &value)
{
    auto result = m_varList.find(name);
    if(result != m_varList.end())
    {
        TechniqueVar * var =  result->second;
        var->setF(value);
    }else
    {
        TechniqueVar * var = new TechniqueVar();
        var->setF(value);
        m_varList.insert(std::make_pair(name,var));
    }
}

/**
 * @brief Technique::setVar 设置Technique的变量
 * @param name 变量的类型
 * @param value 值
 * @note Technique会自动缓存这些变量，只要Technique还在生存周期内，这些值都会不断的提交到shader
 */
void Technique::setVar(std::string name, const int &value)
{
    auto result = m_varList.find(name);
    if(result != m_varList.end())
    {
        TechniqueVar * var =  result->second;
        var->setI(value);
    }else
    {
        TechniqueVar * var = new TechniqueVar();
        var->setI(value);
        m_varList.insert(std::make_pair(name,var));
    }
}

/**
 * @brief Technique::setVar 设置Technique的变量
 * @param name 变量的类型
 * @param value 值
 * @note Technique会自动缓存这些变量，只要Technique还在生存周期内，这些值都会不断的提交到shader
 */
void Technique::setVar(std::string name, const vec3 &value)
{
    auto result = m_varList.find(name);
    if(result != m_varList.end())
    {
        TechniqueVar * var =  result->second;
        var->setV3(value);
    }else
    {
        TechniqueVar * var = new TechniqueVar();
        var->setV3(value);
        m_varList.insert(std::make_pair(name,var));
    }
}

/**
 * @brief Technique::setVar 设置Technique的变量
 * @param name 变量的类型
 * @param value 值
 * @note Technique会自动缓存这些变量，只要Technique还在生存周期内，这些值都会不断的提交到shader
 */
void Technique::setVar(std::string name, const vec4 & value)
{
    auto result = m_varList.find(name);
    if(result != m_varList.end())
    {
        TechniqueVar * var =  result->second;
        var->setV4(value);
    }else
    {
        TechniqueVar * var = new TechniqueVar();
        var->setV4(value);
        m_varList.insert(std::make_pair(name,var));
    }
}

/**
 * @brief Technique::setTex 设置Technique的纹理变量
 * @param name 采样器名称
 * @param texture 纹理对象
 * @param id 纹理单元ID
 * @note 纹理是一种特殊的变量，因此只能使用该接口进行设置，要提交一份纹理给shader访问，首先必须要指明其纹理的名称——通过参数texture
 * 其次要指定其在shader里采样器的名字——通过参数name，最后还要知道当前纹理需要占用显卡的第几个纹理单元（也是采样器对象的值）——通过参数id，通常来说，默认情况下
 * Opengl会打开第一个纹理单元，因此该函数的id参数的默认值0
 */
void Technique::setTex(std::string name, Texture *texture, int id)
{
    auto result = m_varList.find(name);
    if(result != m_varList.end())
    {
        TechniqueVar * var =  result->second;
        var->setT(texture);
        var->data.i = id;
    }else
    {
        TechniqueVar * var = new TechniqueVar();
        var->setT(texture);
        var->data.i = id;
        m_varList.insert(std::make_pair(name,var));
    }
}
/**
 * @brief Technique::use 使用当前technique所维护的shader进行渲染，并将technique维护的所有值提交到
 * shader端
 */
void Technique::use()
{
    m_program->use();
    for(auto i = m_varList.begin();i!= m_varList.end();i++)
    {
        std::string name = i->first;
        TechniqueVar* var = i->second;
        switch(var->type)
        {
        case TechniqueVar::Type::Float:
            m_program->setUniformFloat(name.c_str(),var->data.f);
            break;
        case TechniqueVar::Type::Integer:
            m_program->setUniformInteger(name.c_str(),var->data.i);
            break;
        case TechniqueVar::Type::Matrix:
            m_program->setUniformMat4v(name.c_str(),var->data.m.data());
            break;
        case TechniqueVar::Type::Vec4:
        {
            auto v = var->data.v4;
            m_program->setUniform4Float(name.c_str(),v);
        }
            break;
        case TechniqueVar::Type::Vec3:
        {
            auto v = var->data.v3;
            m_program->setUniform3Float(name.c_str(),v);
        }
            break;
        case TechniqueVar::Type::Vec2:
        {
            auto v = var->data.v2;
            m_program->setUniform2Float(name.c_str(),v.x,v.y);
        }
            break;
        case TechniqueVar::Type::Texture:
        {
            //first bind the texture obj to specified texture unit
            //then pass the texture unit index to the shader's specified sampler.
            auto tex = var->data.tex;
            auto id = var->data.i;
            RenderBackEnd::shared()->bindTexture2D(id,tex->handle(),tex->getType());
            m_program->setUniformInteger(name.c_str(),id);
        }
            break;
        }
    }

}

ShaderProgram *Technique::program()
{
    return m_program;
}


/**
 * @brief Technique::isExist 判断该Technique对象是否正在维护指定名称的变量
 * @param name 变量的名称
 * @return 存在返回true，反之false
 */
bool Technique::isExist(std::string name)
{
    auto result = m_varList.find(name);
    if(result != m_varList.end())
    {
        return true;
    }else
    {
        return false;
    }
}

} // namespace tzw

