#include "ScriptPyMgr.h"


#include "Lua/lua.hpp"
#include "LuaBridge/LuaBridge.h"

#include "Scene/SceneMgr.h"
#include "ScriptBase.h"
#include "Base/Log.h"
#include "Engine/Engine.h"
#include "Event/EventMgr.h"

#include "ScriptBinding.h"

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
		int res = luaL_dofile(g_lua_state,"./Res/Script/tzw.lua");
	    if (res != 0) {
			const char* error = lua_tostring(g_lua_state, -1);//´òÓ¡´íÎó½á¹û
			printf("%s\n",error);
			lua_pop(g_lua_state, 1); 
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
			printf("error : %s\n", lua_tostring(g_lua_state, -1)); 
		}
	}

	void ScriptPyMgr::doScriptInit()
	{
		lua_getglobal(g_lua_state, "tzw_engine_init");
		if (lua_pcall(g_lua_state, 0, 0, 0) != 0)
		{
			printf("error : %s\n", lua_tostring(g_lua_state, -1)); 
		}
	}

	void ScriptPyMgr::doScriptUIUpdate()
	{
		lua_getglobal(g_lua_state, "tzw_engine_ui_update");
		lua_pushnumber(g_lua_state, Engine::shared()->deltaTime());
		lua_pcall(g_lua_state, 1, 0, 0);
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
		printf("error : %s\n", lua_tostring(g_lua_state, -1)); 
		}
		if(luaL_loadfile(g_lua_state,"./Res/Script/tzw.lua")!= 0)
		{
			printf("error : %s\n", lua_tostring(g_lua_state, -1)); 
		}
	}

	void* ScriptPyMgr::getState()
	{
		return g_lua_state;
	}
}
