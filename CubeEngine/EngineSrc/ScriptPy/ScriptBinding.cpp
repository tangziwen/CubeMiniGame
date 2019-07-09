
#include "ScriptBinding.h"

#include "Lua/lua.hpp"
#include "LuaBridge/LuaBridge.h"

#include "Scene/SceneMgr.h"
#include "ScriptBase.h"
#include "Base/Log.h"
#include "Engine/Engine.h"
#include "Event/EventMgr.h"

#include "ScriptPyMgr.h"
#include "2D/imgui.h"

#define BIND_PROP(className, PROP) .addProperty(#PROP, &className## ::##PROP)
#define BIND_FUNC(className, FUNC) .addFunction(#FUNC, &className## ::##FUNC)
#define BIND_START(state) luabridge::getGlobalNamespace(state)
#define BIND_BEGIN_CLASS(className) .beginClass<className>(#className)

#define BIND_END_CLASS .endClass();
namespace tzw
{

	bool imgui_begin(const char* name, int flags)
	{
		bool isOpen = true;
		ImGui::Begin(name, &isOpen, flags);
		return isOpen;
	}

	void imgui_text(const char* name)
	{
		ImGui::Text(name);
	}

	void g_init_engine_libs()
	{
		auto luaState = static_cast<lua_State *>(ScriptPyMgr::shared()->getState());

		//Vec2
		BIND_START(luaState)
		BIND_BEGIN_CLASS(vec2)
		.addConstructor <void (*) ()> ()
		BIND_PROP(vec2, x)
		BIND_PROP(vec2, y)
		BIND_END_CLASS

		//Vec3
		BIND_START(luaState)
		BIND_BEGIN_CLASS(vec3)
		.addConstructor <void (*) ()> ()
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
		.addFunction("Begin", &imgui_begin)
		.addFunction("End", &ImGui::End)
		BIND_FUNC(ImGui, SmallButton)
		//BIND_FUNC(ImGui, SliderFloat)
		BIND_FUNC(ImGui, Button)
		BIND_FUNC(ImGui, BeginMenu)
		BIND_FUNC(ImGui, EndMenu)
		//BIND_FUNC(ImGui, MenuItem)
		BIND_FUNC(ImGui, BeginPopup)
		BIND_FUNC(ImGui, EndPopup)
		
		BIND_FUNC(ImGui, BeginGroup)
		BIND_FUNC(ImGui, EndGroup)
		.addFunction("Text", static_cast<void (*)(int)>(ImGui::PushID))
		BIND_FUNC(ImGui, PopID)
		BIND_FUNC(ImGui, BeginDragDropTarget)
		BIND_FUNC(ImGui, EndDragDropTarget)
		BIND_FUNC(ImGui, Separator)
		.addFunction("Text", static_cast<bool (*)(const char*, bool)>(ImGui::RadioButton))
		.addFunction("Text", &imgui_text)
		BIND_FUNC(ImGui, SameLine)
		BIND_FUNC(ImGui, ColorButton)
		.endNamespace ();

		//Scene
		
		BIND_START(luaState)
		.addFunction("g_GetCurrScene", &g_GetCurrScene);
		
	}
}
