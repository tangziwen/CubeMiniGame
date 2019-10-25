#pragma once
#include "Engine/EngineDef.h"
#include <string>


namespace tzw
{
	class EventInfo;
	class ScriptPyMgr
	{
		TZW_SINGLETON_DECL(ScriptPyMgr)
	public:
		ScriptPyMgr();
		void init();
		void doScriptInit();
		void doScriptUIUpdate();
		void finalize();
		void raiseInputEvent(EventInfo eventInfo);
		void callFunV(std::string funcName);
		bool callFunB(std::string funcName);
		std::string runString(std::string theStr);
		void reload();
		void * getState();
	private:

	};
}
