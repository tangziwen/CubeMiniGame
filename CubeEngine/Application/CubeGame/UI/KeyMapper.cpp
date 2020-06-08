#include "KeyMapper.h"
// #include "imgui.h"

#include "Utility/log/Log.h"
#include "Engine/Engine.h"
#define NOMINMAX
#include "dirent.h"
#include "Base/TranslationMgr.h"
#include "Event/EventMgr.h"
namespace tzw
{
	void KeyMapper::drawIMGUI(bool* isOpen)
	{
		ImGui::SetNextWindowFocus();
		auto screenSize = Engine::shared()->winSize();
		ImGui::SetNextWindowPos(ImVec2(screenSize.x / 2.0, screenSize.y / 2.0), ImGuiCond_Always, ImVec2(0.5, 0.5));
		ImGui::Begin("KeyMapper",0,ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration);
		GUISystem::shared()->imguiUseLargeFont();
		ImGui::Text("Please Press Any Key To Bind");
		ImGui::PopFont();
		ImGui::End();
	}

	KeyMapper::KeyMapper():m_isOpen(false), m_changeCB(nullptr)
	{
		setIsSwallow(true);
		setFixedPiority(9999);
		EventMgr::shared()->addFixedPiorityListener(this);
	}

	void KeyMapper::open(std::function<void(int)> keyChangeCallBack)
	{
		m_changeCB = keyChangeCallBack;
		m_isOpen = true;
	}

	bool KeyMapper::isOpen()
	{
		return m_isOpen;
	}

	void KeyMapper::hide()
	{
		m_isOpen = false;
		m_changeCB = nullptr;
	}

	bool KeyMapper::onKeyRelease(int keyCode)
	{
		if(m_isOpen)
		{
			switch(keyCode)
			{
			case TZW_KEY_ESCAPE:
				hide();
				return true;
				break;
			default:
				if(m_changeCB)
				{
					m_changeCB(keyCode);
				}
				hide();
				return true;
				break;
			
			}
		}

		return false;
	}

}
