#include "ScriptPyMgr.h"


#include "Lua/lua.hpp"
#include "LuaBridge/LuaBridge.h"

#include "Scene/SceneMgr.h"
#include "ScriptBase.h"
#include "Utility/log/Log.h"
#include "Engine/Engine.h"
#include "Event/EventMgr.h"

#include "ScriptBinding.h"
#include "Utility/file/Tfile.h"
#include "rapidjson/document.h"

namespace tzw
{ 
	TZW_SINGLETON_IMPL(ScriptPyMgr);
	lua_State * g_lua_state = nullptr;
	ScriptPyMgr::ScriptPyMgr() 
	{

	}

	struct test_calss_a
	{
		int data;
		test_calss_a(int newVal)
		{
			data = newVal;
		}
	};

	struct test_calss_b
	{
		test_calss_b()
		{
			
		}
		void printHeiHei(test_calss_a*  ptr)
		{
			printf("on Print Hei Hei Hei %d\n", ptr->data);
		}
	};
	void test_func(test_calss_a * ptr)
	{
		printf("aaaaaa %d\n", ptr->data);
	}

	void ScriptPyMgr::init()
	{
		g_lua_state = luaL_newstate();
		luaL_openlibs(g_lua_state);
		g_init_engine_libs();
		luabridge::getGlobalNamespace(g_lua_state).beginNamespace ("test").addFunction("test_func", test_func).endNamespace();

		rapidjson::Document doc;
		std::string filePath = "Data/Module/ModConfig.json";
		auto data = Tfile::shared()->getData(filePath, true);
		doc.Parse<rapidjson::kParseDefaultFlags>(data.getString().c_str());
		if (doc.HasParseError())
		{
			tlog("[error] get json data err! %s %d offset %d\n", filePath.c_str(), doc.GetParseError(), doc.GetErrorOffset());
			exit(1);
		}
		auto& items = doc["Mods"];
		for (unsigned int i = 0; i < items.Size(); i++)
		{
			auto& item = items[i];
			char scriptEntryName[1024];
			sprintf(scriptEntryName,"%s%s%s","Data/Module/", item["Name"].GetString(), "/Script/tzw.lua");
			int res = luaL_dofile(g_lua_state,scriptEntryName);
		    if (res != 0) 
			{
				const char* error = lua_tostring(g_lua_state, -1);//打印错误结果
				printf("%s\n",error);
				lua_pop(g_lua_state, 1); 
			}
		}


	}

	void ScriptPyMgr::finalize()
	{

	}

	void ScriptPyMgr::raiseInputEvent(EventInfo eventInfo)
	{
		lua_getglobal(g_lua_state, "tzw_engine_input_event");
		//lua_pushinteger(g_lua_state, input_type);
		//lua_pushinteger(g_lua_state, input_code);
		luabridge::push(g_lua_state, eventInfo);
		if (lua_pcall(g_lua_state, 1, 0, 0) != 0)
		{
			tlogError("error : %s\n", lua_tostring(g_lua_state, -1)); 
		}
	}

	void ScriptPyMgr::callFunV(std::string funcName)
	{
		lua_getglobal(g_lua_state, funcName.c_str());
		if (lua_pcall(g_lua_state, 0, 0, 0) != 0)
		{
			tlogError("error : %s\n", lua_tostring(g_lua_state, -1)); 
		}
	}

	bool ScriptPyMgr::callFunB(std::string funcName)
	{
		lua_getglobal(g_lua_state, funcName.c_str());

		if (lua_pcall(g_lua_state, 0, 1, 0) != 0)
		{
			tlogError("error : %s\n", lua_tostring(g_lua_state, -1));
		}
		bool returnVal = lua_toboolean(g_lua_state, -1);
		lua_pop(g_lua_state, 1);
		return returnVal;
	}

	bool ScriptPyMgr::callFun1IB(std::string funcName, int arg)
	{
		lua_getglobal(g_lua_state, funcName.c_str());
		luabridge::push(g_lua_state, arg);
		if (lua_pcall(g_lua_state, 1, 1, 0) != 0)
		{
			tlogError("error : %s\n", lua_tostring(g_lua_state, -1));
		}
		bool returnVal = lua_toboolean(g_lua_state, -1);
		lua_pop(g_lua_state, 1);
		return returnVal;
	}

	void ScriptPyMgr::doScriptInit()
	{
		lua_getglobal(g_lua_state, "tzw_engine_init");
		if (lua_pcall(g_lua_state, 0, 0, 0) != 0)
		{
			tlogError("error : %s\n", lua_tostring(g_lua_state, -1)); 
		}
	}

	void ScriptPyMgr::doScriptUIUpdate()
	{
		lua_getglobal(g_lua_state, "tzw_engine_ui_update");
		lua_pushnumber(g_lua_state, Engine::shared()->deltaTime());
		if(lua_pcall(g_lua_state, 1, 0, 0))
		{
			tlogError("error : %s\n", lua_tostring(g_lua_state, -1)); 
		}
	}

	std::string ScriptPyMgr::runString(std::string theStr)
	{
		return "";
	}

	void ScriptPyMgr::reload()
	{
		lua_getglobal(g_lua_state, "tzw_engine_reload");
		if (lua_pcall(g_lua_state, 0, 0, 0) != 0)
		{
			tlogError("error on Reload:\n %s\n", lua_tostring(g_lua_state, -1)); 
		}
	}

	void* ScriptPyMgr::getState()
	{
		return g_lua_state;
	}

	void ScriptPyMgr::doString(std::string s)
	{
		int res = luaL_dostring(g_lua_state,s.c_str());
	    if (res!= 0) 
		{
			const char* error = lua_tostring(g_lua_state, -1);//打印错误结果
			tlogError("%s\n",error);
			lua_pop(g_lua_state, 1); 
		}
	}
}
