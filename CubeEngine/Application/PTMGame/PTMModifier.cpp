#include "PTMModifier.h"
namespace tzw
{
	float& PTMModifier::operator[](std::string paramName)
	{
		return m_params[paramName.c_str()];
	}

	void PTMModifier::reset()
	{
		m_params.clear();
	}

	void PTMModifierContainer::add(PTMModifier* mod)
	{
		m_modifiedMap[mod->getName()] = mod;
		eval();
	}

	void PTMModifierContainer::addButNoEval(PTMModifier* mod)
	{
		m_modifiedMap[mod->getName()] = mod;
	}

	void PTMModifierContainer::delByName(std::string name)
	{
		auto iter = m_modifiedMap.find(name);
		if(iter != m_modifiedMap.end())
		{
			m_modifiedMap.erase(iter);
			//return iter->second;
		}
		eval();
	}
	void PTMModifierContainer::reset()
	{
		m_modifiedMap.clear();
	}
	void PTMModifierContainer::eval()
	{
		m_paramsChache.clear();
		for(auto &iterMod : m_modifiedMap)
		{
			for(auto &iterParam : iterMod.second->m_params)
			{
				auto iterParamCache  = m_paramsChache.find(iterParam.first);
				if(iterParamCache!= m_paramsChache.end())
				{
					iterParamCache->second += iterParam.second;
				}
				else
				{
					m_paramsChache[iterParam.first] = iterParam.second; 
				}
			}
		}
	}
	float PTMModifierContainer::operator[](std::string paramName)
	{
		auto iter = m_paramsChache.find(paramName);
		if(iter != m_paramsChache.end())
		{
			return iter->second;
		}
		else
		{
			return 0.0;
		}
	}
}