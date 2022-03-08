
#include "PTMConfig.h"
#include <filesystem>
#include "Utility/file/Tfile.h"
#include "Utility/log/Log.h"

namespace tzw
{

	PTMConfigMgr G_PTMConfigMgr;
	void PTMConfig::loadFromFile(std::string filePath)
	{
		rapidjson::Document doc;
		auto data = Tfile::shared()->getData(filePath, true);
		doc.Parse<rapidjson::kParseTrailingCommasFlag | rapidjson::kParseCommentsFlag>(data.getString().c_str());
		if (doc.HasParseError())
		{
			tlog("[error] get json data err! %s %d offset %d",
				filePath.c_str(),
				doc.GetParseError(),
				doc.GetErrorOffset());
			exit(1);
		}

		for(auto i = doc.MemberBegin(); i!= doc.MemberEnd(); i++)
		{
			i->name;
			i->value;
			m_params[i->name.GetString()] = i->value.GetFloat();
		}
	}

	float PTMConfig::operator[](const std::string& key)
	{
		return m_params[key];
	}

	PTMConfig& PTMConfigMgr::get(std::string configName)
	{
		auto iter = m_configs.find(configName);
		if(iter == m_configs.end())//load config
		{
			PTMConfig config;
			
			std::string prefix = "./PTM/data/";
			std::string postfix = ".json";
			config.loadFromFile(prefix + configName + postfix);
			m_configs[configName] = config;
			return m_configs[configName];
		}
		else
		{
			return iter->second;
		}
	}

	PTMConfig& PTMConfigMgr::operator[](const std::string& configName)
	{
		return get(configName);
	}
	

}
