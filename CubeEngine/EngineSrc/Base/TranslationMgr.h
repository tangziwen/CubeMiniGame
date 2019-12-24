#pragma once
#include "Engine/EngineDef.h"
#include <map>

namespace tzw {
	class TranslationMgr
	{
	public:
		TZW_SINGLETON_DECL(TranslationMgr);
		TranslationMgr();
		std::string getStr(std::string);
		void dump();
		void load(std::string languageName);
		std::string getCurrLanguage();
	private:
		std::string m_languageName;
		std::map<std::string, std::string> m_currDict;
	};
} // namespace tzw
#define TR(theString) TranslationMgr::shared()->getStr(theString)
#define TRC(theString) TR(theString).c_str()