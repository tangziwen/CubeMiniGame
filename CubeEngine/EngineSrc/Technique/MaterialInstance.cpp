#include "MaterialInstance.h"
#include "MaterialTemplate.h"
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
#include "Scene/SceneMgr.h"
#include <sstream> 

namespace tzw {
namespace
{

std::string resolvePathInFolder(const std::string& filePath, const std::string& envFolder)
{
	auto filePathInFolder = Tfile::shared()->toAbsFilePath(filePath, envFolder);
	if(Tfile::shared()->isExist(filePathInFolder))
	{
		return filePathInFolder;
	}
	return filePath;
}

void applyAttributeOverride(MaterialInstance * material, rapidjson::Value& attribute)
{
	if(!attribute.HasMember("name") || !attribute.HasMember("type"))
	{
		return;
	}

	std::string name = attribute["name"].GetString();
	std::string typeStr = attribute["type"].GetString();
	rapidjson::Value * val = nullptr;
	if(attribute.HasMember("value"))
	{
		val = &attribute["value"];
	}
	else if(attribute.HasMember("default"))
	{
		val = &attribute["default"];
	}

	if(!val)
	{
		return;
	}

	if(typeStr == "int")
	{
		material->setVar(name, val->GetInt());
	}
	else if(typeStr == "float")
	{
		material->setVar(name, static_cast<float>(val->GetDouble()));
	}
	else if(typeStr == "vec2")
	{
		material->setVar(name, vec2((*val)[0].GetDouble(), (*val)[1].GetDouble()));
	}
	else if(typeStr == "vec3")
	{
		material->setVar(name, vec3((*val)[0].GetDouble(), (*val)[1].GetDouble(), (*val)[2].GetDouble()));
	}
	else if(typeStr == "vec4")
	{
		material->setVar(name, vec4((*val)[0].GetDouble(), (*val)[1].GetDouble(), (*val)[2].GetDouble(), (*val)[3].GetDouble()));
	}
}

}

MaterialInstance::MaterialInstance(): m_materialTemplate(nullptr),m_isMaterialTemplateUnique(false),m_shadingParams(nullptr)
{
}

void MaterialInstance::loadFromTemplate(std::string name)
{
	//tlog("load MaterialInstance %s\n", name.c_str());
	applyTemplate(MaterialTemplate::getFromTemplate(name));
	//tlog("load MaterialInstance finished %s\n", name.c_str());
}

void MaterialInstance::loadFromFile(std::string filePath)
{
	rapidjson::Document doc;
	auto data = Tfile::shared()->getData(filePath, true);
	doc.Parse<rapidjson::kParseDefaultFlags>(data.getString().c_str());
	if (doc.HasParseError())
	{
		tlog("[error] get json data err! %s %d offset %d", filePath.c_str(), doc.GetParseError(), doc.GetErrorOffset());
		abort();
	}
	std::string envFolder = Tfile::shared()->getFolder(filePath);
	if(doc.HasMember("MaterialInstance"))
	{
		loadFromInstanceJson(doc, envFolder);
	}
	else
	{
		applyTemplate(MaterialTemplate::getFromFile(filePath));
	}
}

void MaterialInstance::loadFromJson(rapidjson::Value& doc, std::string envFolder)
{
	if(doc.HasMember("MaterialInstance"))
	{
		loadFromInstanceJson(doc, envFolder);
		return;
	}

	auto materialTemplate = new MaterialTemplate();
	materialTemplate->loadFromJson(doc, envFolder);
	applyTemplate(materialTemplate);
}

void MaterialInstance::applyTemplate(MaterialTemplate * materialTemplate)
{
	if(!materialTemplate)
	{
		tlog("[error] bad material template!!!");
		abort();
	}

	m_materialTemplate = materialTemplate;
	m_isMaterialTemplateUnique = false;
	m_name = materialTemplate->m_name;

	if(!m_shadingParams)
	{
		m_shadingParams = new ShadingParams();
	}
	m_shadingParams->copyFrom(materialTemplate->m_shadingParams);
}

void MaterialInstance::loadFromInstanceJson(rapidjson::Value& doc, std::string envFolder)
{
	std::string materialPath = resolvePathInFolder(doc["MaterialInstance"].GetString(), envFolder);
	applyTemplate(MaterialTemplate::getFromFile(materialPath));
	if(doc.HasMember("overrides"))
	{
		applyInstanceOverrides(doc["overrides"], envFolder);
	}
}

void MaterialInstance::applyInstanceOverrides(rapidjson::Value& overrides, std::string envFolder)
{
	if(overrides.HasMember("attributes"))
	{
		auto& attributes = overrides["attributes"];
		for(unsigned int i = 0; i < attributes.Size(); i++)
		{
			applyAttributeOverride(this, attributes[i]);
		}
	}

	if(overrides.HasMember("maps"))
	{
		auto& texMap = overrides["maps"];
		for(unsigned int i = 0; i < texMap.Size(); i++)
		{
			auto& tex = texMap[i];
			std::string name = tex[0].GetString();
			std::string texturePath;
			if(tex[1].IsString())
			{
				texturePath = tex[1].GetString();
			}
			else if(tex[1].IsInt())
			{
				ensureUniqueMaterialTemplate()->m_texSlotMap[name] = tex[1].GetInt();
				if(tex.Size() > 2 && tex[2].IsString())
				{
					texturePath = tex[2].GetString();
				}
			}
			if(!texturePath.empty())
			{
				texturePath = resolvePathInFolder(texturePath, envFolder);
				setTex(name, TextureMgr::shared()->getByPath(texturePath, true));
			}
		}
	}

}

MaterialInstance *MaterialInstance::createFromTemplate(std::string name)
{
	auto mat = new MaterialInstance();
	mat->loadFromTemplate(name);
	return mat;
}

MaterialInstance * MaterialInstance::createFromFile(std::string matPath)
{
	auto mat = new MaterialInstance();
	mat->loadFromFile(matPath);
	return mat;
}

MaterialInstance* MaterialInstance::createFromJson(rapidjson::Value& obj, std::string envFolder)
{
	auto mat = new MaterialInstance();
	mat->loadFromJson(obj, envFolder);
	return mat;
}

/**
 * @brief Technique::setVar 设置Technique的变量
 * @param name 变量的类型
 * @param value 值
 * @note Technique会自动缓存这些变量，只要Technique还在生存周期内，这些值都会不断的提交到shader
 */
void MaterialInstance::setVar(std::string name, const Matrix44 & value)
{
	m_shadingParams->setVar(name, value);
}

/**
 * @brief Technique::setVar 设置Technique的变量
 * @param name 变量的类型
 * @param value 值
 * @note Technique会自动缓存这些变量，只要Technique还在生存周期内，这些值都会不断的提交到shader
 */
void MaterialInstance::setVar(std::string name, const float &value)
{
	m_shadingParams->setVar(name, value);
}

/**
 * @brief Technique::setVar 设置Technique的变量
 * @param name 变量的类型
 * @param value 值
 * @note Technique会自动缓存这些变量，只要Technique还在生存周期内，这些值都会不断的提交到shader
 */
void MaterialInstance::setVar(std::string name, const int &value)
{
	m_shadingParams->setVar(name, value);
}

 void MaterialInstance::setVar(std::string name, const vec2 & value)
 {
	 m_shadingParams->setVar(name, value);
 }

/**
 * @brief Technique::setVar 设置Technique的变量
 * @param name 变量的类型
 * @param value 值
 * @note Technique会自动缓存这些变量，只要Technique还在生存周期内，这些值都会不断的提交到shader
 */
void MaterialInstance::setVar(std::string name, const vec3 &value)
{
	m_shadingParams->setVar(name, value);
}

/**
 * @brief Technique::setVar 设置Technique的变量
 * @param name 变量的类型
 * @param value 值
 * @note Technique会自动缓存这些变量，只要Technique还在生存周期内，这些值都会不断的提交到shader
 */
void MaterialInstance::setVar(std::string name, const vec4 & value)
{
	m_shadingParams->setVar(name, value);
}

void MaterialInstance::setVar(std::string name, const TechniqueVar &value)
{
	m_shadingParams->setVar(name, value);
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
void MaterialInstance::setTex(std::string name, Texture *texture, int id)
{
	m_shadingParams->setTex(name, texture, id);

}

Texture* MaterialInstance::getTex(std::string name)
{
	return m_shadingParams->getTex(name);
}

/**
 * @brief Technique::use 使用当前technique所维护的shader进行渲染，并将technique维护的所有值提交到
 * shader端
 */
void MaterialInstance::use(ShaderProgram * extraProgram)
{
	ShaderProgram * program = getProgram();
	if (extraProgram)
	{
		program = extraProgram;
	}
	program->use();
	/*
	for(auto &i : m_varList)
	{
		//need to convert to alias
		const std::string &name = i.first;//getAlias(i->first);
		TechniqueVar* var = &i.second;

		//extra semantic pass
		switch(var->type)
		{
			case TechniqueVar::Type::Float:
				program->setUniformFloat(name.c_str(),var->data.rawData.f);
			break;
			case TechniqueVar::Type::Integer:
				program->setUniformInteger(name.c_str(),var->data.rawData.i);
			break;
			case TechniqueVar::Type::Matrix:
				program->setUniformMat4v(name.c_str(),var->data.rawData.m.data());
			break;
			case TechniqueVar::Type::Vec4:
				{
					auto v = var->data.rawData.v4;
					program->setUniform4Float(name.c_str(),v);
				}
			break;
			case TechniqueVar::Type::Vec3:
				{
					auto v = var->data.rawData.v3;
					program->setUniform3Float(name.c_str(),v);
				}
			break;
			case TechniqueVar::Type::Vec2:
				{
					auto v = var->data.rawData.v2;
					program->setUniform2Float(name.c_str(),v.x,v.y);
				}
			break;
			case TechniqueVar::Type::Texture:
				{
					//first bind the texture obj to specified texture unit
					//then pass the texture unit index to the shader's specified sampler.
					auto tex = var->data.rawData.texInfo.tex;
					if(!tex)//use default texture to avoid problem
					{
						tex = TextureMgr::shared()->getByPath("Texture/BuiltInTexture/defaultBaseColor.png");
					}
					auto id = getMapSlot(name);
					RenderBackEnd::shared()->bindTexture2DAndUnit(id,tex->handle()->m_uid,tex->getType());
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
	*/
}


unsigned int MaterialInstance::getMapSlot(std::string mapName)
{
	return m_materialTemplate->getMapSlot(mapName);
}


ShaderProgram *MaterialInstance::getProgram() const
{
	return m_materialTemplate->getProgram();
}


MaterialInstance *MaterialInstance::clone()
{
	auto mat = new MaterialInstance();
	mat->m_materialTemplate = m_materialTemplate;
	mat->m_isMaterialTemplateUnique = false;
	mat->m_shadingParams = new ShadingParams();
	if(m_shadingParams)
	{
		mat->m_shadingParams->copyFrom(*m_shadingParams);
	}
	mat->m_name = m_name;
	return mat;
}

void MaterialInstance::reload()
{
	m_materialTemplate->reload();
}

bool MaterialInstance::getIsCullFace()
{
	return m_materialTemplate->getIsCullFace();
}

RenderFlag::CullMode MaterialInstance::getCullMode()
{
	return m_materialTemplate->getCullMode();
}

TechniqueVar * MaterialInstance::get(std::string name)
{
	return m_shadingParams->getVar(name);
}

void MaterialInstance::inspect()
{
	for(auto &iter:m_shadingParams->getVarList())
	{
		auto& var = iter.second;
		switch (var.type)
		{

		case TechniqueVar::Type::Float:
		{
	
			bindFloat(iter.first, &var.data.rawData.f,var.data.f_min, var.data.f_max, "%.2f");
	
		}
			break;


		default:
			break;
		}
	}
}

void MaterialInstance::inspectIMGUI(std::string name, float min, float max, const char * fmt /*= "%.2f"*/)
{
	float uvSize = get(name)->data.rawData.f;
	ImGui::SliderFloat(name.c_str(), &uvSize, min, max, fmt);
	setVar(name, uvSize);
}

void MaterialInstance::inspectIMGUI_Color(std::string name)
{
	auto src = get(name)->data.rawData.v3;
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

void MaterialInstance::handleSemanticValuePassing(TechniqueVar * val, const std::string name, ShaderProgram * program)
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
		case TechniqueVar::SemanticType::CamPos:
		{
			program->setUniform3Float(name.c_str(), g_GetCurrScene()->defaultCamera()->getWorldPos());
		}
		break;
        case TechniqueVar::SemanticType::InvertedViewProj:
		{
			auto currScene = g_GetCurrScene();
			auto cam = currScene->defaultCamera();
        	program->setUniformMat4v(name.c_str(), cam->getViewProjectionMatrix().inverted().data());
        }
		break;
		case TechniqueVar::SemanticType::CamDir:
		{
			program->setUniform3Float(name.c_str(), g_GetCurrScene()->defaultCamera()->getForward());
		}
		break;
		case TechniqueVar::SemanticType::CamInfo:
		{
			auto currScene = g_GetCurrScene();
			auto cam = currScene->defaultCamera();
        	program->setUniform4Float(name.c_str(), vec4(cam->getNear(), cam->getFar(), cam->getFov(), cam->getAspect()));
        }
		break;
		default: ;
	}
}

RenderFlag::BlendingFactor MaterialInstance::getFactorSrc() const
{
	return m_materialTemplate->getFactorSrc();
}

RenderFlag::BlendingFactor MaterialInstance::getFactorDst() const
{
	return m_materialTemplate->getFactorDst();
}

bool MaterialInstance::isIsEnableBlend() const
{
	return m_materialTemplate->isIsEnableBlend();
}

uint32_t MaterialInstance::getMutationFlag()
{
	return m_materialTemplate->getMutationFlag();
}

uint32_t MaterialInstance::getMaterialFlag()
{
	return m_materialTemplate->getMaterialFlag();
}

const std::string& MaterialInstance::getFullDescriptionStr()
{
	return m_materialTemplate->getFullDescriptionStr();
}

std::unordered_map<std::string, TechniqueVar>& MaterialInstance::getVarList()
{
	return m_shadingParams->getVarList();
}

PrimitiveTopology MaterialInstance::getPrimitiveTopology()
{
	return m_materialTemplate->getPrimitiveTopology();
}

RasterFillMode MaterialInstance::getRasterFillMode()
{
	return m_materialTemplate->getRasterFillMode();
}

RenderFlag::RenderStage MaterialInstance::getRenderStage() const
{
	return m_materialTemplate->getRenderStage();
}

bool MaterialInstance::isIsDepthTestEnable() const
{
	return m_materialTemplate->isIsDepthTestEnable();
}

bool MaterialInstance::isIsDepthWriteEnable() const
{
	return m_materialTemplate->isIsDepthWriteEnable();
}

bool MaterialInstance::isEnableInstanced()
{
	return m_materialTemplate->isEnableInstanced();
}

/**
 * @brief Technique::isExist 判断该Technique对象是否正在维护指定名称的变量
 * @param name 变量的名称
 * @return 存在返回true，反之false
 */
bool MaterialInstance::isExist(std::string name)
{
	return m_shadingParams->isVarExist(name);
}

ShadingParams * MaterialInstance::getShadingParams()
{
	return m_shadingParams;
}

MaterialTemplate * MaterialInstance::getMaterialTemplate()
{
	return m_materialTemplate;
}

const MaterialTemplate * MaterialInstance::getMaterialTemplate() const
{
	return m_materialTemplate;
}

MaterialTemplate * MaterialInstance::ensureUniqueMaterialTemplate()
{
	if(!m_isMaterialTemplateUnique)
	{
		m_materialTemplate = m_materialTemplate->clone();
		m_isMaterialTemplateUnique = true;
	}
	return m_materialTemplate;
}

} // namespace tzw

