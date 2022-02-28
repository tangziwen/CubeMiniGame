#pragma once
#include "Engine/EngineDef.h"
#include <unordered_map>
namespace tzw
{

	class PTMModifier
	{
	TZW_PROPERTY(std::string, Name, "");
	public:
		float &operator [](std::string paramName);
		void reset();
	protected:
		std::unordered_map<std::string, float> m_params;
		friend class PTMModifierContainer;
	};

	class PTMModifierContainer
	{
	public:
		void add(PTMModifier * mod);
		void addButNoEval(PTMModifier * mod);
		void delByName(std::string name);
		void reset();
		void eval();
		float operator [](std::string paramName);
	protected:
		std::unordered_map<std::string, PTMModifier *> m_modifiedMap;
		std::unordered_map<std::string, float> m_paramsChache;
	};
}