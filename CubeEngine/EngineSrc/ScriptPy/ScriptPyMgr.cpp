#include "ScriptPyMgr.h"

#include "Lua/lua.hpp"
#include "LuaBridge/LuaBridge.h"

#include "Engine/Engine.h"
#include "Event/EventMgr.h"
#include "Scene/SceneMgr.h"
#include "Utility/log/Log.h"

#include "ScriptBinding.h"
#include "Utility/file/Tfile.h"
#include "rapidjson/document.h"

#include "Python/include/Python.h"
#include "Python/include/import.h"
#include <pybind11/embed.h> // everything needed for embedding
namespace py = pybind11;
namespace tzw
{
	ScriptPyMgr::ScriptPyMgr()
	{
	}


	void
	ScriptPyMgr::init()
	{
	    py::initialize_interpreter();// start the interpreter and keep it alive

	    py::print("Hello, World!"); // use the Python API

		g_lua_state = luaL_newstate();
		luaL_openlibs(g_lua_state);
		g_init_engine_libs();
		rapidjson::Document doc;
		std::string filePath = "Data/Module/ModConfig.json";
		auto data = Tfile::shared()->getData(filePath, true);
		doc.Parse<rapidjson::kParseDefaultFlags>(data.getString().c_str());
		if (doc.HasParseError())
		{
			tlog("[error] get json data err! %s %d offset %d\n",
			     filePath.c_str(),
			     doc.GetParseError(),
			     doc.GetErrorOffset());
			exit(1);
		}
		auto& items = doc["Mods"];
		for (unsigned int i = 0; i < items.Size(); i++)
		{
			auto& item = items[i];
			// char scriptEntryName[1024];
			// sprintf(scriptEntryName,
			//         "%s%s%s",
			//         "Data/Module/",
			//         item["Name"].GetString(),
			//         "/Script/tzw.lua");
			// int res = luaL_dofile(g_lua_state, scriptEntryName);
			// if (res != 0)
			// {
			// 	const char* error = lua_tostring(g_lua_state, -1); //打印错误结果
			// 	printf("%s\n", error);
			// 	lua_pop(g_lua_state, 1);
			// }

			char scriptEntryName_py[1024];
			sprintf(scriptEntryName_py,
			        "%s%s%s",
			        "Data/Module/",
			        item["Name"].GetString(),
			        "/Script/");
			//Set the path
			std::string path = scriptEntryName_py;
			PyRun_SimpleString("import sys");
			std::string str = "sys.path.append('" + path + "')";
			PyRun_SimpleString(str.c_str());
			// auto pyfile = fopen(scriptEntryName_py, "r");
			// PyRun_SimpleFile(pyfile, scriptEntryName_py);
		    auto pName = PyUnicode_DecodeFSDefault("tzw");
		    /* Error checking of pName left out */

		    auto pModule = PyImport_Import(pName);
			if (pModule == nullptr)
			{
			    PyErr_Print();
			}
		}
	}

	void
	ScriptPyMgr::finalize()
	{
	}

	void
	ScriptPyMgr::raiseInputEvent(EventInfo eventInfo)
	{
		// lua_getglobal(g_lua_state, "tzw_engine_input_event");
		// // lua_pushinteger(g_lua_state, input_type);
		// // lua_pushinteger(g_lua_state, input_code);
		// luabridge::push(g_lua_state, eventInfo);
		// if (lua_pcall(g_lua_state, 1, 0, 0) != 0)
		// {
		// 	tlogError("error : %s\n", lua_tostring(g_lua_state, -1));
		// }
		//

		try
		{
			auto module = py::module::import("tzw");
			module.attr("tzw_engine_input_event")(eventInfo);
		}
		catch(py::error_already_set &ex)
		{
			py::print(ex.what());
		}
	}

	void
	ScriptPyMgr::doScriptInit()
	{
		// lua_getglobal(g_lua_state, "tzw_engine_init");
		// if (lua_pcall(g_lua_state, 0, 0, 0) != 0)
		// {
		// 	tlogError("error : %s\n", lua_tostring(g_lua_state, -1));
		// }
		callFunPyVoid("tzw", "tzw_engine_init");
	}

	void
	ScriptPyMgr::doScriptUIUpdate()
	{
		// lua_getglobal(g_lua_state, "tzw_engine_ui_update");
		// lua_pushnumber(g_lua_state, Engine::shared()->deltaTime());
		// if (lua_pcall(g_lua_state, 1, 0, 0))
		// {
		// 	tlogError("error : %s\n", lua_tostring(g_lua_state, -1));
		// }

		callFunPyVoid("tzw", "tzw_engine_ui_update", Engine::shared()->deltaTime());
	}

	std::string
	ScriptPyMgr::runString(std::string theStr)
	{
		return "";
	}

	void
	ScriptPyMgr::reload()
	{
		// lua_getglobal(g_lua_state, "tzw_engine_reload");
		// if (lua_pcall(g_lua_state, 0, 0, 0) != 0)
		// {
		// 	tlogError("error on Reload:\n %s\n", lua_tostring(g_lua_state, -1));
		// }
		callFunPyVoid("tzw", "tzw_engine_reload");
	}

	void*
	ScriptPyMgr::getState()
	{
		return g_lua_state;
	}

	void
	ScriptPyMgr::doString(std::string s)
	{
		int res = luaL_dostring(g_lua_state, s.c_str());
		if (res != 0)
		{
			const char* error = lua_tostring(g_lua_state, -1); //打印错误结果
			tlogError("%s\n", error);
			lua_pop(g_lua_state, 1);
		}
	}
}
