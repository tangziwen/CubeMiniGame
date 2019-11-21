
#include "ScriptBinding.h"

#include "Lua/lua.hpp"
#include "LuaBridge/LuaBridge.h"

#include "Scene/SceneMgr.h"
#include "ScriptBase.h"
#include "utility/log/Log.h"
#include "Engine/Engine.h"
#include "Event/EventMgr.h"

#include "ScriptPyMgr.h"
#include "2D/imgui.h"

#include "2D/imnodes.h"
#include "Base/TranslationMgr.h"
#include "2D/GUISystem.h"

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

	void imgui_ImageButton(unsigned int val, ImVec2 size)
	{
		ImGui::ImageButton(reinterpret_cast<ImTextureID> (val), size);
	}

	void imgui_Image(unsigned int val, ImVec2 size)
	{
		ImGui::Image(reinterpret_cast<ImTextureID> (val), size);
	}
	void imgui_PushStyleColor(int styleEnum, ImVec4 color)
	{
		ImGui::PushStyleColor(styleEnum, color);
	}

	void imgui_PopStyleColor()
	{
		ImGui::PopStyleColor(1);
	}
	std::string translation(std::string theString)
	{
		return TranslationMgr::shared()->getStr(theString);
	}
	void g_init_engine_libs()
	{
		auto luaState = static_cast<lua_State *>(ScriptPyMgr::shared()->getState());

		
		BIND_START(luaState)
		//static function
		.addFunction("TR", &translation)

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
		BIND_FUNC(ImGui, OpenPopup)
		BIND_FUNC(ImGui, GetWindowWidth)
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
