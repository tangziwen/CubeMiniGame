#include "ShadingParams.h"
namespace tzw
{

/**
 * @brief Technique::setVar 设置Technique的变量
 * @param name 变量的类型
 * @param value 值
 * @note Technique会自动缓存这些变量，只要Technique还在生存周期内，这些值都会不断的提交到shader
 */
void ShadingParams::setVar(std::string name, const Matrix44 & value)
{
	auto result = m_varList.find(name);
	if(result != m_varList.end())
	{
		auto & var =  result->second;
		var.setM(value);
	}else
	{
		TechniqueVar var;
		var.setM(value);
		m_varList.insert(std::make_pair(name,var));
	}
}

/**
 * @brief Technique::setVar 设置Technique的变量
 * @param name 变量的类型
 * @param value 值
 * @note Technique会自动缓存这些变量，只要Technique还在生存周期内，这些值都会不断的提交到shader
 */
void ShadingParams::setVar(std::string name, const float &value)
{
	auto result = m_varList.find(name);
	if(result != m_varList.end())
	{
		auto& var =  result->second;
		var.setF(value);
	}else
	{
		TechniqueVar var;
		var.setF(value);
		m_varList.insert(std::make_pair(name,var));
	}
}

/**
 * @brief Technique::setVar 设置Technique的变量
 * @param name 变量的类型
 * @param value 值
 * @note Technique会自动缓存这些变量，只要Technique还在生存周期内，这些值都会不断的提交到shader
 */
void ShadingParams::setVar(std::string name, const int &value)
{
	auto result = m_varList.find(name);
	if(result != m_varList.end())
	{
		auto& var =  result->second;
		var.setI(value);
	}else
	{
		TechniqueVar var;
		var.setI(value);
		m_varList.insert(std::make_pair(name,var));
	}
}

 void ShadingParams::setVar(std::string name, const vec2 & value)
 {
	 auto result = m_varList.find(name);
	 if (result != m_varList.end())
	 {
		 auto &var = result->second;
		 var.setV2(value);
	 }
	 else
	 {
		 TechniqueVar var;
		 var.setV2(value);
		 m_varList.insert(std::make_pair(name, var));
	 }
 }

/**
 * @brief Technique::setVar 设置Technique的变量
 * @param name 变量的类型
 * @param value 值
 * @note Technique会自动缓存这些变量，只要Technique还在生存周期内，这些值都会不断的提交到shader
 */
void ShadingParams::setVar(std::string name, const vec3 &value)
{
	auto result = m_varList.find(name);
	if(result != m_varList.end())
	{
		auto var =  result->second;
		var.setV3(value);
	}else
	{
		TechniqueVar var;
		var.setV3(value);
		m_varList.insert(std::make_pair(name,var));
	}
}

/**
 * @brief Technique::setVar 设置Technique的变量
 * @param name 变量的类型
 * @param value 值
 * @note Technique会自动缓存这些变量，只要Technique还在生存周期内，这些值都会不断的提交到shader
 */
void ShadingParams::setVar(std::string name, const vec4 & value)
{
	auto result = m_varList.find(name);
	if(result != m_varList.end())
	{
		auto &var =  result->second;
		var.setV4(value);
	}else
	{
		TechniqueVar var;
		var.setV4(value);
		m_varList.insert(std::make_pair(name,var));
	}
}

void ShadingParams::setVar(std::string name, const TechniqueVar &value)
{
	TechniqueVar var = value;
	//m_varList.insert(std::make_pair(name, var));
	m_varList[name] = var;
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
void ShadingParams::setTex(std::string name, Texture *texture, int id)
{
	auto result = m_varList.find(name);
	if(result != m_varList.end())
	{
		auto& var =  result->second;
		var.setT(texture, id);
	}else
	{
		TechniqueVar var;
		var.setT(texture, id);
		m_varList.insert(std::make_pair(name,var));
	}
}


Texture* ShadingParams::getTex(std::string name)
{
	auto result = m_varList.find(name);
	if(result == m_varList.end())
	{
		return nullptr;
	}
	return result->second.data.rawData.texInfo.tex;
}


TechniqueVar * ShadingParams::getVar(std::string name)
{
	return &m_varList[name];
}

std::unordered_map<std::string,TechniqueVar> & ShadingParams::getVarList()
{
	// TODO: 在此处插入 return 语句
	return m_varList;
}

bool ShadingParams::isVarExist(std::string name)
{
	auto result = m_varList.find(name);
	return (result != m_varList.end());
}

}