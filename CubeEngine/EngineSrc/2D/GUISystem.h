#pragma once
#include "Event/Event.h"
#include "../Engine/EngineDef.h"
#include "Shader/ShaderProgram.h"
#include "imgui.h"
class EventListener;
namespace tzw
{
	class IMGUIObject
	{
	public:
		virtual ~IMGUIObject() = default;

		virtual void drawIMGUI()
		{
		}
		virtual void drawIMGUI(bool * isOpen)
		{
		}
	};

	class GUISystem: public EventListener
	{
	public:
		TZW_SINGLETON_DECL(GUISystem)
		void NewFrame();
		GUISystem();
		void renderData();
		void initGUI();
		bool onKeyPress(int keyCode) override;
		bool onKeyRelease(int keyCode) override;
		bool ImGui_ImplGlfwGL2_CreateDeviceObjects();
		bool onMouseRelease(int button, vec2 pos) override;
		bool onMousePress(int button, vec2 pos) override;
		void addObject(IMGUIObject * obj);
		bool onCharInput(unsigned int theChar) override;
		bool onScroll(vec2 offset) override;
		void renderIMGUI();
	protected:
		std::vector<IMGUIObject *> m_objList;
		bool m_isInit = false;
		unsigned int tick = 0;
	};
}
