#include "Material.h"
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

void applyAttributeOverride(Material * material, rapidjson::Value& attribute)
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

Material::Material(): m_isCullFace(false), m_program(nullptr),
	m_factorSrc(RenderFlag::BlendingFactor::SrcAlpha),m_factorDst(RenderFlag::BlendingFactor::OneMinusSrcAlpha),
	m_isDepthTestEnable(true), m_isDepthWriteEnable(true), m_isEnableBlend(false),
	m_renderStage(RenderFlag::RenderStage::COMMON),m_isEnableInstanced(false),m_cullMode(RenderFlag::CullMode::Back),
	m_primitiveTopology(PrimitiveTopology::TriangleList),m_rasterFillMode(RasterFillMode::Fill),m_materialTemplate(nullptr),m_shadingParams(nullptr)
{
}

void Material::loadFromTemplate(std::string name)
{
	//tlog("load Material %s\n", name.c_str());
	applyTemplate(MaterialTemplate::getFromTemplate(name));
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
	std::string envFolder = Tfile::shared()->getFolder(filePath);
	if(doc.HasMember("Material"))
	{
		loadFromInstanceJson(doc, envFolder);
	}
	else
	{
		applyTemplate(MaterialTemplate::getFromFile(filePath));
	}
}

void Material::loadFromJson(rapidjson::Value& doc, std::string envFolder)
{
	if(doc.HasMember("Material"))
	{
		loadFromInstanceJson(doc, envFolder);
		return;
	}

	auto materialTemplate = new MaterialTemplate();
	materialTemplate->loadFromJson(doc, envFolder);
	applyTemplate(materialTemplate);
}

void Material::applyTemplate(MaterialTemplate * materialTemplate)
{
	if(!materialTemplate)
	{
		tlog("[error] bad material template!!!");
		abort();
	}

	m_materialTemplate = materialTemplate;
	m_name = materialTemplate->m_name;
	m_vsPath = materialTemplate->m_vsPath;
	m_fsPath = materialTemplate->m_fsPath;
	m_isEnableInstanced = materialTemplate->m_isEnableInstanced;
	m_factorSrc = materialTemplate->m_factorSrc;
	m_factorDst = materialTemplate->m_factorDst;
	m_isCullFace = materialTemplate->m_isCullFace;
	m_isDepthTestEnable = materialTemplate->m_isDepthTestEnable;
	m_isDepthWriteEnable = materialTemplate->m_isDepthWriteEnable;
	m_isEnableBlend = materialTemplate->m_isEnableBlend;
	m_texSlotMap = materialTemplate->m_texSlotMap;
	m_program = materialTemplate->m_program;
	m_renderStage = materialTemplate->m_renderStage;
	m_cullMode = materialTemplate->m_cullMode;
	m_primitiveTopology = materialTemplate->m_primitiveTopology;
	m_rasterFillMode = materialTemplate->m_rasterFillMode;

	if(!m_shadingParams)
	{
		m_shadingParams = new ShadingParams();
	}
	m_shadingParams->copyFrom(materialTemplate->m_shadingParams);
	updateFullDescriptionStr();
}

void Material::loadFromInstanceJson(rapidjson::Value& doc, std::string envFolder)
{
	std::string materialPath = resolvePathInFolder(doc["Material"].GetString(), envFolder);
	applyTemplate(MaterialTemplate::getFromFile(materialPath));
	if(doc.HasMember("overrides"))
	{
		applyInstanceOverrides(doc["overrides"], envFolder);
	}
	updateFullDescriptionStr();
}

void Material::applyInstanceOverrides(rapidjson::Value& overrides, std::string envFolder)
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
				m_texSlotMap[name] = tex[1].GetInt();
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

Material* Material::createFromJson(rapidjson::Value& obj, std::string envFolder)
{
	auto mat = new Material();
	mat->loadFromJson(obj, envFolder);
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
	m_shadingParams->setVar(name, value);
}

/**
 * @brief Technique::setVar 设置Technique的变量
 * @param name 变量的类型
 * @param value 值
 * @note Technique会自动缓存这些变量，只要Technique还在生存周期内，这些值都会不断的提交到shader
 */
void Material::setVar(std::string name, const float &value)
{
	m_shadingParams->setVar(name, value);
}

/**
 * @brief Technique::setVar 设置Technique的变量
 * @param name 变量的类型
 * @param value 值
 * @note Technique会自动缓存这些变量，只要Technique还在生存周期内，这些值都会不断的提交到shader
 */
void Material::setVar(std::string name, const int &value)
{
	m_shadingParams->setVar(name, value);
}

 void Material::setVar(std::string name, const vec2 & value)
 {
	 m_shadingParams->setVar(name, value);
 }

/**
 * @brief Technique::setVar 设置Technique的变量
 * @param name 变量的类型
 * @param value 值
 * @note Technique会自动缓存这些变量，只要Technique还在生存周期内，这些值都会不断的提交到shader
 */
void Material::setVar(std::string name, const vec3 &value)
{
	m_shadingParams->setVar(name, value);
}

/**
 * @brief Technique::setVar 设置Technique的变量
 * @param name 变量的类型
 * @param value 值
 * @note Technique会自动缓存这些变量，只要Technique还在生存周期内，这些值都会不断的提交到shader
 */
void Material::setVar(std::string name, const vec4 & value)
{
	m_shadingParams->setVar(name, value);
}

void Material::setVar(std::string name, const TechniqueVar &value)
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
void Material::setTex(std::string name, Texture *texture, int id)
{
	m_shadingParams->setTex(name, texture, id);

}

Texture* Material::getTex(std::string name)
{
	return m_shadingParams->getTex(name);
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


unsigned int Material::getMapSlot(std::string mapName)
{
	return m_texSlotMap[mapName];
}


ShaderProgram *Material::getProgram() const
{
	return m_program;
}


Material *Material::clone()
{
	auto mat = new Material();
	mat->m_materialTemplate = m_materialTemplate;
	mat->m_shadingParams = new ShadingParams();
	if(m_shadingParams)
	{
		mat->m_shadingParams->copyFrom(*m_shadingParams);
	}
	mat->m_isEnableInstanced = m_isEnableInstanced;
	mat->m_isCullFace = m_isCullFace;
	mat->m_isDepthTestEnable = m_isDepthTestEnable;
	mat->m_isDepthWriteEnable = m_isDepthWriteEnable;
	mat->m_isEnableBlend = m_isEnableBlend;
	mat->m_renderStage = m_renderStage;
	mat->m_program = m_program;
	mat->m_name = m_name;
	mat->m_texSlotMap = m_texSlotMap;
	mat->m_vsPath = m_vsPath;
	mat->m_fsPath = m_fsPath;
	mat->m_factorSrc = m_factorSrc;
	mat->m_factorDst = m_factorDst;
	mat->m_cullMode = m_cullMode;
	mat->m_primitiveTopology = m_primitiveTopology;
	mat->m_rasterFillMode = m_rasterFillMode;
	mat->updateFullDescriptionStr();
	return mat;
}

void Material::reload()
{
	m_program = ShaderMgr::shared()->getByPath(getMutationFlag(), m_vsPath, m_fsPath);
	updateFullDescriptionStr();
}

bool Material::getIsCullFace()
{
	return m_isCullFace;
}

void Material::setIsCullFace(bool newVal)
{
	m_isCullFace = newVal;
	updateFullDescriptionStr();
}

RenderFlag::CullMode Material::getCullMode()
{
	return m_cullMode;
}

void Material::setCullMode(RenderFlag::CullMode newCullMode)
{
	m_cullMode = newCullMode;
	updateFullDescriptionStr();
}

TechniqueVar * Material::get(std::string name)
{
	return m_shadingParams->getVar(name);
}

void Material::inspect()
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

void Material::inspectIMGUI(std::string name, float min, float max, const char * fmt /*= "%.2f"*/)
{
	float uvSize = get(name)->data.rawData.f;
	ImGui::SliderFloat(name.c_str(), &uvSize, min, max, fmt);
	setVar(name, uvSize);
}

void Material::inspectIMGUI_Color(std::string name)
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

RenderFlag::BlendingFactor Material::getFactorSrc() const
{
	return m_factorSrc;
}

void Material::setFactorSrc(const RenderFlag::BlendingFactor factorSrc)
{
	m_factorSrc = factorSrc;
	updateFullDescriptionStr();
}

RenderFlag::BlendingFactor Material::getFactorDst() const
{
	return m_factorDst;
}

void Material::setFactorDst(const RenderFlag::BlendingFactor factorDst)
{
	m_factorDst = factorDst;
	updateFullDescriptionStr();
}

bool Material::isIsEnableBlend() const
{
	return m_isEnableBlend;
}

void Material::setIsEnableBlend(const bool isEnableBlend)
{
	m_isEnableBlend = isEnableBlend;
	updateFullDescriptionStr();
}

uint32_t Material::getMutationFlag()
{
	uint32_t flag = 0 ;
	if(m_isEnableInstanced)
	{
		flag |= (uint32_t)ShaderOption::EnableInstanced;
	}

	if(!m_isCullFace)
	{
		flag |= (uint32_t)ShaderOption::EnableDoubleSide;
	}
	return flag;
}

uint32_t Material::getMaterialFlag()
{
	uint32_t flag = 0 ;
	if(m_isCullFace)
	{
		flag |= MaterialFlag_isCullFace;
	}
	if(m_isEnableInstanced)
	{
		flag |= MaterialFlag_isInstanced;
	}
	if(m_isEnableBlend)
	{
		flag |= MaterialFlag_isBlend;
	}
	if(m_isDepthWriteEnable)
	{
		flag |= MaterialFlag_isDepthWrite;
	}
	if(m_isDepthTestEnable)
	{
		flag |= MaterialFlag_isDepthTest;
	}
	return flag;
}

void Material::updateFullDescriptionStr()
{
	std::ostringstream ostr;
	if(m_program)
	{
		ostr << "shader:" << m_program->m_vertexShader << "|" << m_program->m_fragmentShader;
	}
	else
	{
		ostr << "shader:" << m_vsPath << "|" << m_fsPath;
	}
	ostr << "|mutation:" << getMutationFlag()
		<< "|stage:" << static_cast<int>(m_renderStage)
		<< "|flags:" << static_cast<uint32_t>(getMaterialFlag())
		<< "|topology:" << static_cast<int>(m_primitiveTopology)
		<< "|raster:" << static_cast<int>(m_rasterFillMode)
		<< "|cull:" << static_cast<int>(m_cullMode)
		<< "|srcBlend:" << static_cast<int>(m_factorSrc)
		<< "|dstBlend:" << static_cast<int>(m_factorDst);
	m_fullDescString = ostr.str();
}

const std::string& Material::getFullDescriptionStr()
{
	return m_fullDescString;
}

std::unordered_map<std::string, TechniqueVar>& Material::getVarList()
{
	return m_shadingParams->getVarList();
}

PrimitiveTopology Material::getPrimitiveTopology()
{
	return m_primitiveTopology;
}

void Material::setPrimitiveTopology(PrimitiveTopology newTopology)
{
	m_primitiveTopology = newTopology;
	updateFullDescriptionStr();
}

RasterFillMode Material::getRasterFillMode()
{
	return m_rasterFillMode;
}

void Material::setRasterFillMode(RasterFillMode newMode)
{
	m_rasterFillMode = newMode;
	updateFullDescriptionStr();
}

RenderFlag::RenderStage Material::getRenderStage() const
{
	return m_renderStage;
}

void Material::setRenderStage(const RenderFlag::RenderStage renderStage)
{
	m_renderStage = renderStage;
	updateFullDescriptionStr();
}

bool Material::isIsDepthTestEnable() const
{
	return m_isDepthTestEnable;
}

void Material::setIsDepthTestEnable(const bool isDepthTestEnable)
{
	m_isDepthTestEnable = isDepthTestEnable;
	updateFullDescriptionStr();
}

bool Material::isIsDepthWriteEnable() const
{
	return m_isDepthWriteEnable;
}

void Material::setIsDepthWriteEnable(const bool isDepthWriteEnable)
{
	m_isDepthWriteEnable = isDepthWriteEnable;
	updateFullDescriptionStr();
}

void Material::setIsEnableInstanced(const bool isEnableInstanced)
{
	m_isEnableInstanced = isEnableInstanced;
	updateFullDescriptionStr();
}

bool Material::isEnableInstanced()
{
	return m_isEnableInstanced;
}

/**
 * @brief Technique::isExist 判断该Technique对象是否正在维护指定名称的变量
 * @param name 变量的名称
 * @return 存在返回true，反之false
 */
bool Material::isExist(std::string name)
{
	return m_shadingParams->isVarExist(name);
}

ShadingParams * Material::getShadingParams()
{
	return m_shadingParams;
}

} // namespace tzw

