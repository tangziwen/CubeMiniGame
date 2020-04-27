#pragma once
#include "Event/Event.h"
#include "../Engine/EngineDef.h"
#include "Shader/ShaderProgram.h"
#include "imgui.h"
#include "Base/TranslationMgr.h"
class EventListener;
class ImFont;
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

	class GUISystem: public EventListener, public Singleton<GUISystem>
	{
	public:
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

		bool isUiCapturingInput();
		void imguiUseSmallFont();
		void imguiUseNormalFont();
		void imguiUseLargeFont();
	protected:
		std::vector<IMGUIObject *> m_objList;
		bool m_isInit = false;
		unsigned int tick = 0;
		ImFont * m_fontNormal;
		ImFont * m_fontLarge;
		ImFont * m_fontSmall;
	};
}
