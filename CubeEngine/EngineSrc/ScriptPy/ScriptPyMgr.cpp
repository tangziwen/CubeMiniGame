#include "ScriptPyMgr.h"

#include "Engine/Engine.h"
#include "Event/EventMgr.h"
#include "Scene/SceneMgr.h"
#include "Utility/log/Log.h"

#include "Utility/file/Tfile.h"
#include "rapidjson/document.h"

#include "Python/include/Python.h"
#include "Python/include/import.h"
#include <pybind11/embed.h> // everything needed for embedding
namespace py = pybind11;
namespace tzw
{
	void g_init_engine_libs();

	ScriptPyMgr::ScriptPyMgr()
	{
	}


	void
	ScriptPyMgr::init()
	{
	    g_init_engine_libs();
	    py::initialize_interpreter();// start the interpreter and keep it alive

	    py::print("Hello, World!"); // use the Python API

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
			abort();
		}
		auto& items = doc["Mods"];
		PyRun_SimpleString("import sys");
		char libEntry_py[1024];
		sprintf(libEntry_py, "Data/Lib/");
		std::string libStr = "sys.path.append('" + std::string(libEntry_py) + "')";
		PyRun_SimpleString(libStr.c_str());
		for (unsigned int i = 0; i < items.Size(); i++)
		{
			auto& item = items[i];

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
		callFunPyVoid("tzw", "tzw_engine_init");
	}

	void
	ScriptPyMgr::doScriptUIUpdate()
	{
		callFunPyVoid("tzw", "tzw_engine_ui_update", Engine::shared()->deltaTime());
	}

	void
	ScriptPyMgr::reload()
	{
		callFunPyVoid("tzw", "tzw_engine_reload");
	}
}
