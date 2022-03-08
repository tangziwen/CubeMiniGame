#pragma once
#include "Engine/Engine.h"
#include <unordered_map>
namespace tzw
{
	struct PTMConfig
	{
		void loadFromFile(std::string filePath);
		float operator [](const std::string &);
		std::unordered_map<std::string, float> m_params;
	};

	class PTMConfigMgr
	{
	public:
		PTMConfig& get(std::string configName);
		PTMConfig & operator[](const std::string & configName);
	protected:
		std::unordered_map<std::string, PTMConfig> m_configs;
	};
	extern PTMConfigMgr G_PTMConfigMgr;
}
