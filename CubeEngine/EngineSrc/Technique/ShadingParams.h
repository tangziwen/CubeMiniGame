#pragma once
#include <string>
#include <vector>
#include <map>
#include "../Shader/ShaderProgram.h"
#include "TechniqueVar.h"
#include "../Math/Matrix44.h"
#include "EngineSrc/3D/Effect/Effect.h"
#include "2D/InspectableUI.h"
#include "rapidjson/document.h"
#include "Rendering/RenderFlag.h"
namespace tzw
{
class ShadingParams
{
public:
    void setVar(std::string name, const Matrix44 &value);
    void setVar(std::string name,const float& value);
    void setVar(std::string name,const int& value);
	void setVar(std::string name, const vec2 & value);
    void setVar(std::string name, const vec3& value);
    void setVar(std::string name, const vec4 &value);
    void setVar(std::string name, const TechniqueVar & value);
    void setTex(std::string name,Texture * texture,int id = 0);
    Texture * getTex(std::string name);
    TechniqueVar * getVar(std::string);
    std::unordered_map<std::string,TechniqueVar> & getVarList();
    bool isVarExist(std::string name);
protected:
    std::unordered_map<std::string,TechniqueVar> m_varList;
    std::unordered_map<std::string, unsigned int> m_texSlotMap;
};

}