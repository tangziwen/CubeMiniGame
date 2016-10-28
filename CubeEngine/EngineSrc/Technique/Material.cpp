#include "Material.h"
#include <utility>
#include <stdlib.h>
#include "../Shader/ShaderMgr.h"
#include "../BackEnd/RenderBackEnd.h"

#include "../Interface/Drawable3D.h"
#include "EngineSrc/3D/Effect/EffectMgr.h"
namespace tzw {

/**
 * @brief Technique::Technique 构造函数
 * @param vsFilePath 顶点shader的文件路径
 * @param fsFilePath 片段shader的文件路径
 */
Material::Material()
{

}

Material::Material(Effect *theEffect)
{
	initFromEffect(theEffect);
}

Material *Material::createFromEffect(std::string name)
{
	auto effect = EffectMgr::shared()->get(name);
	return new Material(effect);
}

void Material::initFromEffect(Effect *theEffect)
{
	m_effect = theEffect;
	loadDefaultValues();
}

void Material::initFromEffect(std::string effectName)
{
	auto effect = EffectMgr::shared()->get(effectName);
	initFromEffect(effect);
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
void Material::use()
{
	auto program = m_effect->getProgram();
	program->use();
	for(auto i = m_varList.begin();i!= m_varList.end();i++)
	{
		//need to convert to alias
		std::string name = m_effect->getAlias(i->first);
		TechniqueVar* var = i->second;
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
					auto id = m_effect->getMapSlot(name);
					RenderBackEnd::shared()->bindTexture2D(id,tex->handle(),tex->getType());
					program->setUniformInteger(name.c_str(),id);
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

Material *Material::clone()
{
	auto tech = new Material();
	tech->m_varList = m_varList;
	tech->m_effect = m_effect;
	return tech;
}

Effect *Material::getEffect() const
{
	return m_effect;
}

void Material::setEffect(Effect *value)
{
	m_effect = value;
}

void Material::loadDefaultValues()
{
	std::map<std::string, TechniqueVar> theMap;
	m_effect->getAttrList(theMap);
	for(auto &item : theMap)
	{
		setVar(item.first, item.second);
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

