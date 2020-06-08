#pragma once
#include "2D/GUISystem.h"
#include <functional>

namespace tzw
{
	class KeyMapper : public IMGUIObject, public Singleton<KeyMapper>, EventListener
	{
	public:
		// Í¨¹ý IMGUIObject ¼Ì³Ð
		void drawIMGUI(bool * isOpen) override;
		KeyMapper();
		void open(std::function<void(int)> keyChangeCallBack);
		bool isOpen();
		void hide();
		bool onKeyRelease(int keyCode) override;
    private:
		std::function<void(int)> m_changeCB;
		bool m_isOpen;
	};

}
