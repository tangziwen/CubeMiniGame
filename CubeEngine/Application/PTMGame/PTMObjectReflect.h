#pragma once
#include <string>
#include <unordered_map>


#define PTM_PROP_REFLECT_REG(PROP) m_propsLink[#PROP] = &m_##PROP;//regReflect##PROP();

#define PTM_PROP_REFLECT_REG_BEGIN(CLASS_NAME)\
	void init_reg_reflect_##CLASS_NAME(CLASS_NAME *obj)\
	{

#define PTM_PROP_REFLECT_REG_END }\
protected:\
int m_dullObj = [this](){init_reg_reflect();return 0;}();

#define PTM_PROP_REFLECT_DO(CLASSNAME) init_reg_reflect_##CLASSNAME(this);

#define PTM_PROP_REFLECT_REG_DECLEAR() void init_reg_reflect(){


namespace tzw
{
	class PTMObjectReflect
	{
	public:
		template<class T>
		void setPropByName(std::string name, T val)
		{
			T * iter = (T *)m_propsLink[name];
			(*iter) = val;
		}
		template<class T>
		T getPropByName(std::string name)
		{
			T * iter = (T *)m_propsLink[name];
			return (*iter);
		}

	protected:
		std::unordered_map<std::string, void *> m_propsLink;
	};

}