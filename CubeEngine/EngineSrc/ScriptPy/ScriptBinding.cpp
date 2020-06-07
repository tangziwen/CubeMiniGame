
#include "ScriptBinding.h"

#include "Lua/lua.hpp"
#include "LuaBridge/LuaBridge.h"

#include "Scene/SceneMgr.h"

#include "utility/log/Log.h"
#include "Engine/Engine.h"
#include "Event/EventMgr.h"

#include "ScriptPyMgr.h"
#include "2D/imgui.h"
#include "2D/imgui_internal.h"
#include "2D/imnodes.h"
#include "Base/TranslationMgr.h"
#include "2D/GUISystem.h"
#define BIND_PROP(className, PROP) .addProperty(#PROP, &className## ::##PROP)
#define BIND_FUNC(className, FUNC) .addFunction(#FUNC, &className## ::##FUNC)
#define BIND_START(state) luabridge::getGlobalNamespace(state)
#define BIND_BEGIN_CLASS(className) .beginClass<className>(#className)
#define BIND_END_CLASS .endClass();

// #define BIND_BEGIN_CLASS_PY(className) pybind11::class_<className>(m, #className)
// #define BIND_PROP_PY(className, FUNC) .def_readwrite(#FUNC, &className## ::##FUNC)
// #define BIND_FUNC_PY(className, FUNC) .def(#FUNC, &className## ::##FUNC)
// #define BIND_STATIC_FUNC_PY(className, FUNC) .def_static(#FUNC, &className## ::##FUNC)
// #define BIND_SINGLETON_PY(className) .def_static("shared", &className## ::shared, pybind11::return_value_policy::reference)
//
// #define BIND_MODULE_FUNC_PY(className, FUNC) m.def(#FUNC, &className## ::##FUNC)
// #define BIND_MODULE_CUSTOM_FUNC_PY(FUNCStr, FUNCPtr) m.def(FUNCStr, FUNCPtr)
#include "ScriptPy/ScriptBindMacro.h"
#include <pybind11/embed.h>
namespace py = pybind11;

namespace tzw
{

	bool imgui_begin(const char* name, int flags)
	{
		bool isOpen = true;
		ImGui::Begin(name, &isOpen, flags);
		return isOpen;
	}

	bool imgui_begin_child(const char* name, int x, int y, int flags)
	{
		bool isOpen = true;
		isOpen = ImGui::BeginChild(name, ImVec2(x, y), flags);
		return isOpen;
	}
	bool imgui_begin_no_close(const char* name, int flags)
	{
		return ImGui::Begin(name, NULL, flags);
	}
	void imgui_text(const char* name)
	{
		ImGui::Text(name);
	}

	void imgui_textwrapped(const char* name)
	{
		ImGui::TextWrapped(name);
	}

	bool imgui_begin_popup_modal(const char * name, int flags)
	{
		return ImGui::BeginPopupModal(name,NULL, flags);
	}
	struct imnodes_link_info
	{
		int start_attr;
		int end_attr;
		bool isCreated;
		imnodes_link_info()
		{
			isCreated = false;
			start_attr = 0;
			end_attr = 0;
		}
	};

	imnodes_link_info imnodes_IsLinkCreated()
	{
		int start_attr, end_attr;
		imnodes_link_info info;
		info.isCreated = imnodes::IsLinkCreated(&start_attr, &end_attr);
		info.start_attr = start_attr;
		info.end_attr = end_attr;
		return info;
	}

	int imgui_getPayLoadData2Int(const ImGuiPayload * payLoad)
	{
		
		return *(const int*)payLoad->Data;
	}

	bool imgui_BeginDragDropSource()
	{
		return ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID);
	}

	void imgui_SetDragDropPayload(std::string payloadStr, int val)
	{
		ImGui::SetDragDropPayload("DND_DEMO_CELL", &val, sizeof(int)); 
	}

	bool imgui_ImageButton(unsigned int val, ImVec2 size)
	{
		return ImGui::ImageButton(reinterpret_cast<ImTextureID> (val), size, ImVec2(0, 1), ImVec2(1,0));
	}

	void imgui_Image(unsigned int val, ImVec2 size)
	{
		ImGui::Image(reinterpret_cast<ImTextureID> (val), size, ImVec2(0, 1), ImVec2(1,0));
	}
	
	void imgui_PushStyleColor(int styleEnum, ImVec4 color)
	{
		ImGui::PushStyleColor(styleEnum, color);
	}

	void imgui_PopStyleColor()
	{
		ImGui::PopStyleColor(1);
	}

	void imgui_drawFrame(ImVec2 min, ImVec2 max, float size, ImVec4 color)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, size);
		ImGui::PushStyleColor(ImGuiCol_Border, color);
		ImGui::RenderFrameBorder(min, max, 1);
		ImGui::PopStyleVar(1);
		ImGui::PopStyleColor(1);
	}

	std::string translation(std::string theString)
	{
		return TranslationMgr::shared()->getStr(theString);
	}
	void tlog_lua(std::string myStr)
	{
		tlog(myStr.c_str());
	}
	int l_my_print(lua_State *L) {
	    int nargs = lua_gettop(L);
	    for (int i = 1; i <= nargs; ++i) {
	        tlog(luaL_tolstring(L, i, nullptr));
	        lua_pop(L, 1); // remove the string
	    }
	    return 0;
	}

	void checkLeak(std::string filePath)
	{
		//auto f = fopen("./tzw.txt", "w");
		//nvwa::check_leaks(NULL);
		//fclose(f);
	}

	
	int add(int i, int j) {
	    return i + j;
	}
	namespace py = pybind11;
	PYBIND11_EMBEDDED_MODULE(CubeEngine, m)
	{
	    m.doc() = "tzw"; // optional module docstring
	    m.def("add", &add, "A function which adds two numbers");


		BIND_MODULE_CUSTOM_FUNC_PY("TR", &translation);
		BIND_MODULE_CUSTOM_FUNC_PY("tlog", &tlog_lua);
		
		//vec2
	    BIND_BEGIN_CLASS_PY(vec2)
        .def(py::init<>())
        BIND_PROP_PY(vec2, x)
        BIND_PROP_PY(vec2, y);


		//vec3
	     BIND_BEGIN_CLASS_PY(vec3)
		.def(py::init<>())
		.def("scale", &vec3::scale)
		BIND_PROP_PY(vec3, x)
		BIND_PROP_PY(vec3, y)
		BIND_PROP_PY(vec3, z);
	  //
	    BIND_BEGIN_CLASS_PY(vec4)
		.def(py::init<>())
		BIND_PROP_PY(vec4, x)
		BIND_PROP_PY(vec4, y)
		BIND_PROP_PY(vec4, z)
		BIND_PROP_PY(vec4, w);
	  //
	    py::class_<Engine>(m, "Engine")
			BIND_SINGLETON_PY(Engine)
			BIND_FUNC_PY(Engine, getFilePath)
			BIND_FUNC_PY(Engine, winSize)
			BIND_FUNC_PY(Engine, setUnlimitedCursor);
		 
	    py::class_<Texture>(m, "Texture")
			BIND_FUNC_PY(Texture, handle);
	  
	    py::class_<TextureMgr>(m, "TextureMgr")
			BIND_SINGLETON_PY(TextureMgr)
			BIND_FUNC_PY(TextureMgr, getByPathSimple);
		 
	    py::class_<EventInfo>(m, "EventInfo")
	        .def(py::init<>())
			BIND_PROP_PY(EventInfo, type)
			BIND_PROP_PY(EventInfo, keycode)
			BIND_PROP_PY(EventInfo, theChar)
			BIND_PROP_PY(EventInfo, type)
			BIND_PROP_PY(EventInfo, arg)
			BIND_PROP_PY(EventInfo, pos)
			BIND_PROP_PY(EventInfo, offset);
		

		BIND_BEGIN_CLASS_PY(GUISystem)
		BIND_SINGLETON_PY(GUISystem)
		BIND_FUNC_PY(GUISystem, isUiCapturingInput);

		
		BIND_MODULE_CUSTOM_FUNC_PY("g_GetCurrScene", &g_GetCurrScene);
	}

	//ImGui
	PYBIND11_EMBEDDED_MODULE(ImGui, m)
	{
	    m.doc() = "ImGui"; // optional module docstring

		//ImVec2
		BIND_BEGIN_CLASS_PY(ImVec2)
		.def(py::init<float, float>())
		BIND_PROP_PY(ImVec2, x)
		BIND_PROP_PY(ImVec2, y);

		//ImVec4
		BIND_BEGIN_CLASS_PY(ImVec4)
		.def(py::init<float, float, float, float>())
		BIND_PROP_PY(ImVec4, x)
		BIND_PROP_PY(ImVec4, y)
		BIND_PROP_PY(ImVec4, z)
		BIND_PROP_PY(ImVec4, w);

		BIND_BEGIN_CLASS_PY(ImGuiPayload);
		
		m.def("GetPayLoadData2Int",&imgui_getPayLoadData2Int);
		BIND_MODULE_CUSTOM_FUNC_PY("Begin", &imgui_begin);
		BIND_MODULE_CUSTOM_FUNC_PY("BeginNoClose", &imgui_begin_no_close);
		BIND_MODULE_FUNC_PY(ImGui, End);
		BIND_MODULE_FUNC_PY(ImGui, SmallButton);
		//BIND_FUNC(ImGui, SliderFloat)
		BIND_MODULE_FUNC_PY(ImGui, Button);
		BIND_MODULE_CUSTOM_FUNC_PY("ImageButton", &imgui_ImageButton);
		BIND_MODULE_CUSTOM_FUNC_PY("Image", &imgui_Image);
		BIND_MODULE_FUNC_PY(ImGui, BeginMenu);
		BIND_MODULE_FUNC_PY(ImGui, EndMenu);
		BIND_MODULE_CUSTOM_FUNC_PY("BeginDragDropSource", &imgui_BeginDragDropSource);
		BIND_MODULE_CUSTOM_FUNC_PY("SetDragDropPayload", &imgui_SetDragDropPayload);
		BIND_MODULE_FUNC_PY(ImGui, EndDragDropSource);
		//BIND_FUNC(ImGui, MenuItem)
		BIND_MODULE_FUNC_PY(ImGui, BeginPopup);
		BIND_MODULE_FUNC_PY(ImGui, GetItemRectMax);
		BIND_MODULE_FUNC_PY(ImGui, GetItemRectMin);
		BIND_MODULE_CUSTOM_FUNC_PY("DrawFrame", &imgui_drawFrame);
		BIND_MODULE_FUNC_PY(ImGui, OpenPopup);
		BIND_MODULE_FUNC_PY(ImGui, GetWindowWidth);
		BIND_MODULE_FUNC_PY(ImGui, GetWindowContentRegionMax);
		BIND_MODULE_FUNC_PY(ImGui, GetWindowPos);
		
		BIND_BEGIN_CLASS_PY(ImGuiStyle)
		BIND_PROP_PY(ImGuiStyle, ItemSpacing)
		BIND_PROP_PY(ImGuiStyle, FramePadding)
		BIND_PROP_PY(ImGuiStyle, IndentSpacing);

		
		BIND_MODULE_FUNC_PY(ImGui, GetStyle);
		BIND_MODULE_FUNC_PY(ImGui, EndPopup);
		
		BIND_MODULE_FUNC_PY(ImGui, BeginGroup);
		BIND_MODULE_FUNC_PY(ImGui, EndGroup);
		BIND_MODULE_CUSTOM_FUNC_PY("PushID", static_cast<void (*)(int)>(ImGui::PushID));
		BIND_MODULE_CUSTOM_FUNC_PY("PushID_str", static_cast<void (*)(const char *)>(ImGui::PushID));
		BIND_MODULE_FUNC_PY(ImGui, PopID);
		BIND_MODULE_FUNC_PY(ImGui, BeginDragDropTarget);
		BIND_MODULE_FUNC_PY(ImGui, BeginDragDropTargetAnyWindow);
		BIND_MODULE_FUNC_PY(ImGui, EndDragDropTarget);
		BIND_MODULE_FUNC_PY(ImGui, Separator);
		BIND_MODULE_CUSTOM_FUNC_PY("RadioButton", static_cast<bool (*)(const char*, bool)>(ImGui::RadioButton));
		BIND_MODULE_CUSTOM_FUNC_PY("Text", &imgui_text);
		BIND_MODULE_CUSTOM_FUNC_PY("TextWrapped", &imgui_textwrapped);
		BIND_MODULE_FUNC_PY(ImGui, SameLine);
		BIND_MODULE_FUNC_PY(ImGui, Spacing);;
		BIND_MODULE_FUNC_PY(ImGui, SetNextWindowPos);
		BIND_MODULE_FUNC_PY(ImGui, SetNextWindowSize);
		BIND_MODULE_FUNC_PY(ImGui, ColorButton);
		BIND_MODULE_FUNC_PY(ImGui, CloseCurrentPopup);
		BIND_MODULE_FUNC_PY(ImGui, BeginDragDropTarget);
		BIND_MODULE_FUNC_PY(ImGui, EndDragDropTarget);
		BIND_MODULE_FUNC_PY(ImGui, EndDragDropTarget);
		BIND_MODULE_FUNC_PY(ImGui, AcceptDragDropPayload);
		BIND_MODULE_FUNC_PY(ImGui, Columns);
		BIND_MODULE_FUNC_PY(ImGui, NextColumn);
		BIND_MODULE_CUSTOM_FUNC_PY("BeginChild", &imgui_begin_child);
		BIND_MODULE_FUNC_PY(ImGui, EndChild);
		BIND_MODULE_CUSTOM_FUNC_PY("PushStyleColor", &imgui_PushStyleColor);
		BIND_MODULE_CUSTOM_FUNC_PY("PopStyleColor", &imgui_PopStyleColor);
		BIND_MODULE_CUSTOM_FUNC_PY("BeginPopupModal", &imgui_begin_popup_modal);
		BIND_MODULE_CUSTOM_FUNC_PY("CollapsingHeader", static_cast<bool (*)(const char*, int)>(ImGui::CollapsingHeader));

    }
	void g_init_engine_libs()
	{
		//
		auto luaState = static_cast<lua_State *>(ScriptPyMgr::shared()->getState());

	    lua_pushcfunction(luaState, l_my_print);
	    lua_setglobal(luaState, "print");
		BIND_START(luaState)
		//static function
		.addFunction("TR", &translation)
		.addFunction("tlog", &tlog_lua)
		.addFunction("checkLeak", &checkLeak)
		BIND_BEGIN_CLASS(vec2)//Vec2
		.addConstructor <void (*) ()> ()
		BIND_PROP(vec2, x)
		BIND_PROP(vec2, y)
		BIND_END_CLASS

		//Vec3
		BIND_START(luaState)
		BIND_BEGIN_CLASS(vec3)
		.addConstructor <void (*) ()> ()
		.addFunction("__add", &vec3::operator+)
		.addFunction("scale", &vec3::scale)
		BIND_PROP(vec3, x)
		BIND_PROP(vec3, y)
		BIND_PROP(vec3, z)
		BIND_END_CLASS


		//Vec4
		BIND_START(luaState)
		BIND_BEGIN_CLASS(vec4)
		.addConstructor <void (*) ()> ()
		BIND_PROP(vec4, x)
		BIND_PROP(vec4, y)
		BIND_PROP(vec4, z)
		BIND_END_CLASS


		//EventInfo
		BIND_START(luaState)
		BIND_BEGIN_CLASS(EventInfo)
		.addConstructor <void (*) ()> ()
		BIND_PROP(EventInfo, type)
		BIND_PROP(EventInfo, keycode)
		BIND_PROP(EventInfo, theChar)
		BIND_PROP(EventInfo, arg)
		BIND_PROP(EventInfo, pos)
		BIND_END_CLASS

		//Engine
		BIND_START(luaState)
		BIND_BEGIN_CLASS(Engine)
		.addStaticFunction ("shared", &Engine::shared)
		BIND_FUNC(Engine, getFilePath)
		BIND_FUNC(Engine, winSize)
		BIND_FUNC(Engine, setUnlimitedCursor)
		BIND_END_CLASS

		BIND_START(luaState)
		BIND_BEGIN_CLASS(Texture)
		BIND_FUNC(Texture, handle)
		.endClass()
		BIND_BEGIN_CLASS(TextureMgr)
		.addStaticFunction ("shared", &TextureMgr::shared)
		.addFunction("getByPathSimple", &TextureMgr::getByPathSimple)
		BIND_END_CLASS

		//IMGUI
		BIND_START(luaState)
		.beginNamespace("ImGui")
		BIND_BEGIN_CLASS(ImVec2)
		.addConstructor <void (*) (float, float)> ()
		BIND_PROP(ImVec2, x)
		BIND_PROP(ImVec2, y)
		.endClass()

	
		BIND_BEGIN_CLASS(ImVec4)
		.addConstructor <void (*) (float, float, float, float)> ()
		BIND_PROP(ImVec4, x)
		BIND_PROP(ImVec4, y)
		BIND_PROP(ImVec4, z)
		BIND_PROP(ImVec4, w)
		.endClass()
		BIND_BEGIN_CLASS(ImGuiPayload)
		.endClass()
		.addFunction("GetPayLoadData2Int",&imgui_getPayLoadData2Int)
		.addFunction("Begin", &imgui_begin)
		.addFunction("BeginNoClose", &imgui_begin_no_close)
		.addFunction("End", &ImGui::End)
		BIND_FUNC(ImGui, SmallButton)
		//BIND_FUNC(ImGui, SliderFloat)
		BIND_FUNC(ImGui, Button)
		.addFunction("ImageButton", imgui_ImageButton)
		.addFunction("Image", imgui_Image)
		BIND_FUNC(ImGui, BeginMenu)
		BIND_FUNC(ImGui, EndMenu)
		.addFunction("BeginDragDropSource", &imgui_BeginDragDropSource)
		.addFunction("SetDragDropPayload", &imgui_SetDragDropPayload)
		BIND_FUNC(ImGui, EndDragDropSource)
		//BIND_FUNC(ImGui, MenuItem)
		BIND_FUNC(ImGui, BeginPopup)
		BIND_FUNC(ImGui, GetItemRectMax)
		BIND_FUNC(ImGui, GetItemRectMin)
		.addFunction("DrawFrame", &imgui_drawFrame)
		BIND_FUNC(ImGui, OpenPopup)
		BIND_FUNC(ImGui, GetWindowWidth)
		BIND_FUNC(ImGui, GetWindowContentRegionMax)
		BIND_FUNC(ImGui, GetWindowPos)
		BIND_BEGIN_CLASS(ImGuiStyle)
		BIND_PROP(ImGuiStyle, ItemSpacing)
		BIND_PROP(ImGuiStyle, FramePadding)
		BIND_PROP(ImGuiStyle, IndentSpacing)
		.endClass()
		BIND_FUNC(ImGui, GetStyle)
		BIND_FUNC(ImGui, EndPopup)
		
		BIND_FUNC(ImGui, BeginGroup)
		BIND_FUNC(ImGui, EndGroup)
		.addFunction("PushID", static_cast<void (*)(int)>(ImGui::PushID))
		.addFunction("PushID_str", static_cast<void (*)(const char *)>(ImGui::PushID))
		BIND_FUNC(ImGui, PopID)
		BIND_FUNC(ImGui, BeginDragDropTarget)
		BIND_FUNC(ImGui, BeginDragDropTargetAnyWindow)
		BIND_FUNC(ImGui, EndDragDropTarget)
		BIND_FUNC(ImGui, Separator)
		.addFunction("RadioButton", static_cast<bool (*)(const char*, bool)>(ImGui::RadioButton))
		.addFunction("Text", &imgui_text)
		.addFunction("TextWrapped", &imgui_textwrapped)
		BIND_FUNC(ImGui, SameLine)
		BIND_FUNC(ImGui, Spacing)
		BIND_FUNC(ImGui, SetNextWindowPos)
		BIND_FUNC(ImGui, SetNextWindowSize)
		BIND_FUNC(ImGui, ColorButton)
		BIND_FUNC(ImGui, CloseCurrentPopup)
		BIND_FUNC(ImGui, BeginDragDropTarget)
		BIND_FUNC(ImGui, EndDragDropTarget)
		BIND_FUNC(ImGui, EndDragDropTarget)
		BIND_FUNC(ImGui, AcceptDragDropPayload)
		BIND_FUNC(ImGui, Columns)
		BIND_FUNC(ImGui, NextColumn)
		.addFunction("BeginChild", &imgui_begin_child)
		BIND_FUNC(ImGui, EndChild)
		.addFunction("PushStyleColor", &imgui_PushStyleColor)
		.addFunction("PopStyleColor", &imgui_PopStyleColor)
		.addFunction("BeginPopupModal", &imgui_begin_popup_modal)
		.addFunction("CollapsingHeader", static_cast<bool (*)(const char*, int)>(ImGui::CollapsingHeader))
		.endNamespace ();


		//IMGUI Node
		BIND_START(luaState)
		.beginNamespace("imnodes")
		BIND_FUNC(imnodes, BeginNodeEditor)
		BIND_FUNC(imnodes, BeginNode)
		BIND_FUNC(imnodes, Name)
		BIND_FUNC(imnodes, BeginInputAttribute)
		BIND_FUNC(imnodes, EndAttribute)
		BIND_FUNC(imnodes, BeginOutputAttribute)
		BIND_FUNC(imnodes, EndNode)
		BIND_FUNC(imnodes, Link)
		BIND_FUNC(imnodes, EndNodeEditor)
		//BIND_FUNC(imnodes, IsLinkCreated)
		.addFunction("IsLinkCreated", imnodes_IsLinkCreated)
		.endNamespace();

		BIND_START(luaState)
		BIND_BEGIN_CLASS(imnodes_link_info)
		.addConstructor <void (*) ()> ()
		BIND_PROP(imnodes_link_info, start_attr)
		BIND_PROP(imnodes_link_info, end_attr)
		BIND_PROP(imnodes_link_info, isCreated)
		BIND_END_CLASS
		//Scene

		BIND_START(luaState)
		BIND_BEGIN_CLASS(GUISystem)
		.addStaticFunction ("shared", &GUISystem::shared)
		BIND_FUNC(GUISystem, isUiCapturingInput)
		BIND_END_CLASS

		BIND_START(luaState)
		.addFunction("g_GetCurrScene", &g_GetCurrScene);
		
	}
}
