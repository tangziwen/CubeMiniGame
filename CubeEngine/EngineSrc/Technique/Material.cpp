#include "Material.h"
#include <utility>
#include <stdlib.h>
#include "../Shader/ShaderMgr.h"
#include "../BackEnd/RenderBackEnd.h"

#include "../Interface/Drawable3D.h"
#include "EngineSrc/3D/Effect/EffectMgr.h"
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include "Utility/log/Log.h"
#include "EngineSrc/Shader/ShaderMgr.h"
#include "2D/GUISystem.h"
#include "Texture/TextureMgr.h"
#include "Utility/file/Tfile.h"
#include "Engine/Engine.h"

namespace tzw {

Material::Material(): m_isCullFace(false), m_program(nullptr)
{
}

void Material::loadFromTemplate(std::string name)
{
	//tlog("load Material %s\n", name.c_str());
	loadFromFile(std::string("./Res/MatTemplate/") + name + ".mat");
	//tlog("load Material finished %s\n", name.c_str());
}

void Material::loadFromFile(std::string filePath)
{
	rapidjson::Document doc;
	auto data = Tfile::shared()->getData(filePath, true);
	doc.Parse<rapidjson::kParseDefaultFlags>(data.getString().c_str());
	if (doc.HasParseError())
	{
		tlog("[error] get json data err! %s %d offset %d", filePath.c_str(), doc.GetParseError(), doc.GetErrorOffset());
		abort();
	}
	if (doc.HasMember("name"))
	{
		m_name = doc["name"].GetString();
	}

	if (doc.HasMember("cullFace"))
	{
		m_isCullFace = doc["cullFace"].GetBool();
	}
	else
	{
		m_isCullFace = true;
	}

	if (doc.HasMember("shaders"))
	{
		auto& shaders = doc["shaders"];
		auto vsFilePath = shaders["vs"].GetString();
		auto fsFilePath = shaders["fs"].GetString();
		m_program = ShaderMgr::shared()->getByPath(vsFilePath, fsFilePath);
		if (!m_program)
		{
			tlog("[error] bad program!!!");
			exit(1);
		}
	}
	auto& MaterialInfo = doc["property"];
	if (MaterialInfo.HasMember("attributes"))
	{
		auto& attributes = MaterialInfo["attributes"];
		for (unsigned int i = 0; i < attributes.Size(); i++)
		{
			auto& attribute = attributes[i];

			auto theName = attribute["name"].GetString();
			auto aliasName = theName;
			auto& val = attribute["default"];
			bool hasDefaultVal = true;
			if (val.IsArray() && val.Size() <= 0)
				hasDefaultVal = false;
			std::string typeStr = attribute["type"].GetString();
			m_aliasMap[theName] = aliasName;
			auto var = new TechniqueVar();
			if (typeStr == "int")
			{
				if(hasDefaultVal) 
				{
					var->setI(val.GetInt());
				}
				if(attribute.HasMember("ui_info"))
				{
					auto&uiInfo = attribute["ui_info"];
					if(uiInfo.HasMember("range"))
					{
						var->data.i_min = uiInfo["range"][0].GetInt();
						var->data.i_max = uiInfo["range"][1].GetInt();
					}
				}
			}
			else if (typeStr == "float")
			{

				if(hasDefaultVal) 
				{
					var->setF(val.GetDouble());
                }
				if(attribute.HasMember("ui_info"))
				{
					auto&uiInfo = attribute["ui_info"];
					if(uiInfo.HasMember("range"))
					{
						var->data.f_min = uiInfo["range"][0].GetDouble();
						var->data.f_max = uiInfo["range"][1].GetDouble();
					}
				}
			}
			else if (typeStr == "vec2")
			{
				if(hasDefaultVal)
				{
					var->setV2(vec2(val[0].GetDouble(), val[1].GetDouble()));
				}
			}
			else if (typeStr == "vec3")
			{
				if(hasDefaultVal) 
				{
					var->setV3(vec3(val[0].GetDouble(), val[1].GetDouble(), val[2].GetDouble()));
				}
			}
			else if (typeStr == "vec4")
			{
				if(hasDefaultVal) 
				{
					var->setV4(vec4(val[0].GetDouble(), val[1].GetDouble(), val[2].GetDouble(), val[3].GetDouble()));
				}
			}
			//semantics
			else if(typeStr.find("semantic_") == 0)
			{
				if(typeStr == "semantic_WinSize")
				{
					var->setAsSemantic(TechniqueVar::SemanticType::WIN_SIZE);
				}
				else if(typeStr == "semantic_Model")
				{
					var->setAsSemantic(TechniqueVar::SemanticType::Model);
				}
			}
			m_varList[theName] = var;
		}
	}

	if (MaterialInfo.HasMember("maps"))
	{
		auto& texMap = MaterialInfo["maps"];
		for (unsigned int i = 0; i < texMap.Size(); i++)
		{
			auto& tex = texMap[i];
			auto name = tex[0].GetString();
			auto aliasName = tex[1].GetString();
			m_aliasMap[name] = aliasName;
			m_texSlotMap[m_aliasMap[name]] = tex[2].GetInt();

			if(tex.Size() > 3)
			{
				setTex(name, TextureMgr::shared()->getByPath(tex[3].GetString(), true));
			}
		}
	}
}
Material *Material::createFromTemplate(std::string name)
{
	auto mat = new Material();
	mat->loadFromTemplate(name);
	return mat;
}

Material * Material::createFromFile(std::string matPath)
{
	auto mat = new Material();
	mat->loadFromFile(matPath);
	return mat;
}

/**
 * @brief Technique::setVar 设置Technique的变量
 * @param name 变量的类型
 * @param value 值
 * @note Technique会自动缓存这些变量，只要Technique还在生存周期内，这些值都会不断的提交到shader
 */
void Material::setVar(std::string name, const Matrix44 & value)
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
void Material::setVar(std::string name, const float &value)
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
void Material::setVar(std::string name, const int &value)
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

 void Material::setVar(std::string name, const vec2 & value)
 {
	 auto result = m_varList.find(name);
	 if (result != m_varList.end())
	 {
		 TechniqueVar * var = result->second;
		 var->setV2(value);
	 }
	 else
	 {
		 TechniqueVar * var = new TechniqueVar();
		 var->setV2(value);
		 m_varList.insert(std::make_pair(name, var));
	 }
 }

/**
 * @brief Technique::setVar 设置Technique的变量
 * @param name 变量的类型
 * @param value 值
 * @note Technique会自动缓存这些变量，只要Technique还在生存周期内，这些值都会不断的提交到shader
 */
void Material::setVar(std::string name, const vec3 &value)
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
void Material::setVar(std::string name, const vec4 & value)
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

void Material::setVar(std::string name, const TechniqueVar &value)
{
	TechniqueVar * var = value.clone();
	m_varList.insert(std::make_pair(name, var));
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
void Material::setTex(std::string name, Texture *texture, int id)
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
void Material::use(ShaderProgram * extraProgram)
{
	ShaderProgram * program = m_program;
	if (extraProgram)
	{
		program = extraProgram;
	}
	program->use();
	for(auto i = m_varList.begin();i!= m_varList.end();++i)
	{
		//need to convert to alias
		std::string name = getAlias(i->first);
		TechniqueVar* var = i->second;

		//extra semantic pass
		switch(var->type)
		{
			case TechniqueVar::Type::Float:
				program->setUniformFloat(name.c_str(),var->data.f);
			break;
			case TechniqueVar::Type::Integer:
				program->setUniformInteger(name.c_str(),var->data.i);
			break;
			case TechniqueVar::Type::Matrix:
				program->setUniformMat4v(name.c_str(),var->data.m.data());
			break;
			case TechniqueVar::Type::Vec4:
				{
					auto v = var->data.v4;
					program->setUniform4Float(name.c_str(),v);
				}
			break;
			case TechniqueVar::Type::Vec3:
				{
					auto v = var->data.v3;
					program->setUniform3Float(name.c_str(),v);
				}
			break;
			case TechniqueVar::Type::Vec2:
				{
					auto v = var->data.v2;
					program->setUniform2Float(name.c_str(),v.x,v.y);
				}
			break;
			case TechniqueVar::Type::Texture:
				{
					//first bind the texture obj to specified texture unit
					//then pass the texture unit index to the shader's specified sampler.
					auto tex = var->data.tex;
					if(!tex) break;
					auto id = getMapSlot(name);
					RenderBackEnd::shared()->bindTexture2DAndUnit(id,tex->handle(),tex->getType());
					program->setUniformInteger(name.c_str(),id);
				}
			break;
			case TechniqueVar::Type::Semantic:
				{
					handleSemanticValuePassing(var, name, program);
				}
			break;
			case TechniqueVar::Type::Invalid:
				{
					// do nothing
				}
			break;
		}
	}
}


unsigned int Material::getMapSlot(std::string mapName)
{
	return m_texSlotMap[mapName];
}

unsigned int Material::getMapSlotWithAlias(std::string mapName)
{
	return getMapSlot(getAlias(mapName));
}

std::string Material::getAlias(std::string theName)
{
	auto result = m_aliasMap.find(theName);
	if (result == m_aliasMap.end())
	{
		return theName;
	}
	else
	{
		return m_aliasMap[theName];
	}
}

ShaderProgram *Material::getProgram() const
{
	return m_program;
}


Material *Material::clone()
{
	auto mat = new Material();
	mat->m_varList = m_varList;
	return mat;
}

bool Material::getIsCullFace()
{
	return m_isCullFace;
}

void Material::setIsCullFace(bool newVal)
{
	m_isCullFace = newVal;
}

tzw::TechniqueVar * Material::get(std::string name)
{
	return m_varList[name];
}

void Material::inspect()
{
	for(auto &iter:m_varList)
	{
		auto var = iter.second;
		switch (var->type)
		{

		case TechniqueVar::Type::Float:
		{
	
			bindFloat(iter.first, &var->data.f,var->data.f_min, var->data.f_max, "%.2f");
	
		}
			break;


		default:
			break;
		}
	}
}

void Material::inspectIMGUI(std::string name, float min, float max, const char * fmt /*= "%.2f"*/)
{
	float uvSize = get(name)->data.f;
	ImGui::SliderFloat(name.c_str(), &uvSize, min, max, fmt);
	setVar(name, uvSize);
}

void Material::inspectIMGUI_Color(std::string name)
{
	auto src = get(name)->data.v3;
	static ImVec4 color = ImVec4(src.x, src.y, src.z, 1.0);
	static bool alpha_preview = false;
	static bool alpha_half_preview = false;
	static bool drag_and_drop = true;
	static bool options_menu = true;
	static bool hdr = false;
	int misc_flags = (hdr ? ImGuiColorEditFlags_HDR : 0) | (drag_and_drop ? 0 : 1<<9) | (alpha_half_preview ? ImGuiColorEditFlags_AlphaPreviewHalf : (alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0)) | (options_menu ? 0 : ImGuiColorEditFlags_NoOptions);
	ImGui::ColorEdit3(name.c_str(), (float*)&color, misc_flags);
	setVar(name, vec3(color.x, color.y, color.z));
}

void Material::handleSemanticValuePassing(TechniqueVar * val, const std::string name, ShaderProgram * program)
{
	switch(val->semantic)
	{
		case TechniqueVar::SemanticType::NO_SEMANTIC: break;
        case TechniqueVar::SemanticType::WIN_SIZE:
		{
        	program->setUniform2Float(name.c_str(), Engine::shared()->winSize());
        }
		break;
		case TechniqueVar::SemanticType::ModelViewProj: break;
		case TechniqueVar::SemanticType::Model: break;
		case TechniqueVar::SemanticType::View: break;
		case TechniqueVar::SemanticType::Project: break;
		case TechniqueVar::SemanticType::InvertedProj: break;
		case TechniqueVar::SemanticType::CamPos: break;
		default: ;
	}
}

/**
 * @brief Technique::isExist 判断该Technique对象是否正在维护指定名称的变量
 * @param name 变量的名称
 * @return 存在返回true，反之false
 */
bool Material::isExist(std::string name)
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

