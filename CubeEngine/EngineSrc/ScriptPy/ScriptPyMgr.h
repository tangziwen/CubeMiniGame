#pragma once
#include "Engine/EngineDef.h"
#include <string>

namespace tzw
{
	class ScriptPyMgr
	{
		TZW_SINGLETON_DECL(ScriptPyMgr)
	public:
		ScriptPyMgr();
		void init();
		void doScriptInit();
		void doScriptUpdate();
		void finalize();
		int XRunFunction(const char *module, const char *function,const char *result_format, void *result, const char *args_format, ...);
		void initBuildInModule();
		std::string runString(std::string theStr);
	private:

	};
}