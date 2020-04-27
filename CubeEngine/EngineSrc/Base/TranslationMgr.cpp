#include "TranslationMgr.h"
#include <assert.h>
#include "uuid4.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/prettywriter.h"
#include "Utility/file/Tfile.h"
#include "Utility/log/Log.h"
#include "Engine/Engine.h"

namespace tzw
{
	TranslationMgr::TranslationMgr():m_languageName("CNS")
	{

	}

	std::string TranslationMgr::getStr(std::string theString)
	{
		auto result = m_currDict.find(theString);
		if(result != m_currDict.end())
		{
			return result->second;
		}
		//record it
		m_currDict[theString] = theString;
		return theString;
	}

	void TranslationMgr::dump()
	{
		rapidjson::Document doc;
		doc.SetObject();
		rapidjson::StringBuffer buffer;
		std::string filePath = "./Translation_" + m_languageName + ".json";
		rapidjson::Value stringList(rapidjson::kArrayType);
		for(auto val : m_currDict)
		{
			rapidjson::Value theStringItem(rapidjson::kObjectType);
			theStringItem.AddMember("original", val.first, doc.GetAllocator());
			theStringItem.AddMember("translated", val.second, doc.GetAllocator());
			stringList.PushBack(theStringItem, doc.GetAllocator());
		}
		doc.AddMember("stringList", stringList, doc.GetAllocator());
		auto file = fopen(filePath.c_str(), "w");
		char writeBuffer[65536];
		rapidjson::FileWriteStream stream(file, writeBuffer, sizeof(writeBuffer));
		rapidjson::PrettyWriter<rapidjson::FileWriteStream> writer(stream);
		writer.SetIndent('\t', 1);
		doc.Accept(writer);
		fclose(file);
	}

	void TranslationMgr::load(std::string languageName)
	{
		m_languageName = languageName;
		rapidjson::Document doc;
		std::string filePath = "Translation/" + m_languageName + ".json";
		auto data = Tfile::shared()->getData(filePath, true);
		doc.Parse<rapidjson::kParseDefaultFlags>(data.getString().c_str());
		if (doc.HasParseError())
		{
			tlog("[error] get json data err! %s %d offset %d",
				filePath.c_str(),
				doc.GetParseError(),
				doc.GetErrorOffset());
			exit(1);
		}
		auto& stringList = doc["stringList"];
		for(int i = 0; i < stringList.Size(); i++)
		{
			auto& stringItem = stringList[i];
			m_currDict[stringItem["original"].GetString()] = stringItem["translated"].GetString();
		}
	}

	std::string TranslationMgr::getCurrLanguage()
	{
		return m_languageName;
	}
} // namespace tzw

