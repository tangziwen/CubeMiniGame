#pragma once
#include "Engine/EngineDef.h"
#include <string>

#include "Lua/lua.hpp"
#include "LuaBridge/LuaBridge.h"
#include "Utility/log/Log.h"
namespace tzw
{
	class EventInfo;
	class ScriptPyMgr : public Singleton<ScriptPyMgr>
	{
	public:
		ScriptPyMgr();
		void init();
		void doScriptInit();
		void doScriptUIUpdate();
		void finalize();
		void raiseInputEvent(EventInfo eventInfo);

		
        void pushArg()
        {
            
        }
		template<typename T,class... Types>
        void pushArg(T t, Types... args)
        {
			luabridge::push(g_lua_state, t);
        	pushArg(args...);
        }

        template<class... Types>
		void callFunVoid(const char* functionName, Types ... args);

        template<class... Types>
		bool callFunBool(const char* functionName, Types ... args);

        template<class... Types>
		int callFunInt(const char* functionName, Types ... args);

		std::string runString(std::string theStr);
		void reload();
		void * getState();
		void doString(std::string);
	private:
		lua_State * g_lua_state;

	};

	template <class ... Types>
	void ScriptPyMgr::callFunVoid(const char* functionName, Types ... args)
	{
		lua_getglobal(g_lua_state, functionName);
		size_t size = sizeof...(args);
		pushArg(args...);
		if (lua_pcall(g_lua_state, size, 0, 0) != 0)
		{
			tlogError("error : %s\n", lua_tostring(g_lua_state, -1));
		}
	}

	template <class ... Types>
	bool ScriptPyMgr::callFunBool(const char* functionName, Types ... args)
	{
		lua_getglobal(g_lua_state, functionName);
		size_t size = sizeof...(args);
		pushArg(args...);
		if (lua_pcall(g_lua_state, size, 1, 0) != 0)
		{
			tlogError("error : %s\n", lua_tostring(g_lua_state, -1));
		}
		bool returnVal = lua_toboolean(g_lua_state, -1);
		lua_pop(g_lua_state, 1);
		return returnVal;
	}

	template <class ... Types>
	int ScriptPyMgr::callFunInt(const char* functionName, Types ... args)
	{
		lua_getglobal(g_lua_state, functionName);
		size_t size = sizeof...(args);
		pushArg(args...);
		if (lua_pcall(g_lua_state, size, 1, 0) != 0)
		{
			tlogError("error : %s\n", lua_tostring(g_lua_state, -1));
		}
		int returnVal = lua_tointeger(g_lua_state, -1);
		lua_pop(g_lua_state, 1);
		return returnVal;
	}
}
