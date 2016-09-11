#ifndef TZW_TECHNIQUE_H
#define TZW_TECHNIQUE_H
#include <vector>
#include <string>
#include <map>
#include "../Shader/ShaderProgram.h"
#include "TechniqueVar.h"
#include "../Math/Matrix44.h"

namespace tzw {
class Material;
class Node;
class Drawable;
/**
 * @brief 该类用于维护CPU端向shader提交数据的结构
 *
 * 该类实质上维护了一个shaderProgram 对象以及相关的变量列表，在每次渲染时，technique都会通过Technique::use 函数向shader提交这些变量。
 * 也就是说Technique提供了一种机制将指定的shader与给定的一组变量“绑定”在了一起，这些变量的值可以在shader中以uniform类型变量的方式得以访问。
 * 用户可以使用它再加上自己写好的shader程序实现丰富的效果。
 * Technique的设计思路是泛用的，因此每一个可以被绘制的对象都能够设置Technique（通过Drawable 类），给其指定的值的种类数量都是任意的，
 * 当然对于某些特定情况下，引擎也提供了一些派生类以供简化使用，最常见的派生类为Material(其shader包含光照计算部分的代码，其值主要保存用于计算光照的参数)
 */
class Technique
{
public:
    Technique(const char *vsFilePath,const char *fsFilePath, const char * tcsFilePath = nullptr, const char* tesFilePath = nullptr);
    void setVar(std::string name, const Matrix44 &value);
    void setVar(std::string name,const float& value);
    void setVar(std::string name,const int& value);
    void setVar(std::string name, const vec3& value);
    void setVar(std::string name, const vec4 &value);
    void setTex(std::string name,Texture * texture,int id = 0);
    void use();
    bool isExist(std::string name);
    ShaderProgram * program();
    void applyFromMat(Material *mat);
    void applyFromDrawable(Drawable * node);
private:
    std::string m_vsPath;
    std::string m_fsPath;
    std::map<std::string,TechniqueVar *> m_varList;
    ShaderProgram * m_program;
};

} // namespace tzw

#endif // TZW_TECHNIQUE_H
