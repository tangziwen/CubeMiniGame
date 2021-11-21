#include "ShadingParams.h"
namespace tzw
{

/**
 * @brief Technique::setVar ����Technique�ı���
 * @param name ����������
 * @param value ֵ
 * @note Technique���Զ�������Щ������ֻҪTechnique�������������ڣ���Щֵ���᲻�ϵ��ύ��shader
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
 * @brief Technique::setVar ����Technique�ı���
 * @param name ����������
 * @param value ֵ
 * @note Technique���Զ�������Щ������ֻҪTechnique�������������ڣ���Щֵ���᲻�ϵ��ύ��shader
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
 * @brief Technique::setVar ����Technique�ı���
 * @param name ����������
 * @param value ֵ
 * @note Technique���Զ�������Щ������ֻҪTechnique�������������ڣ���Щֵ���᲻�ϵ��ύ��shader
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
 * @brief Technique::setVar ����Technique�ı���
 * @param name ����������
 * @param value ֵ
 * @note Technique���Զ�������Щ������ֻҪTechnique�������������ڣ���Щֵ���᲻�ϵ��ύ��shader
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
 * @brief Technique::setVar ����Technique�ı���
 * @param name ����������
 * @param value ֵ
 * @note Technique���Զ�������Щ������ֻҪTechnique�������������ڣ���Щֵ���᲻�ϵ��ύ��shader
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
 * @brief Technique::setTex ����Technique���������
 * @param name ����������
 * @param texture �������
 * @param id ����ԪID
 * @note ������һ������ı��������ֻ��ʹ�øýӿڽ������ã�Ҫ�ύһ�������shader���ʣ����ȱ���Ҫָ������������ơ���ͨ������texture
 * ���Ҫָ������shader������������֡���ͨ������name�����Ҫ֪����ǰ������Ҫռ���Կ��ĵڼ�������Ԫ��Ҳ�ǲ����������ֵ������ͨ������id��ͨ����˵��Ĭ�������
 * Opengl��򿪵�һ������Ԫ����˸ú�����id������Ĭ��ֵ0
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
	// TODO: �ڴ˴����� return ���
	return m_varList;
}

bool ShadingParams::isVarExist(std::string name)
{
	auto result = m_varList.find(name);
	return (result != m_varList.end());
}

}