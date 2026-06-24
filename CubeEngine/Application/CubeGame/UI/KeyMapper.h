#pragma once
#include "2D/IMGUISystem.h"
#include <functional>

namespace tzw
{
	class KeyMapper : public IMGUIObject, public Singleton<KeyMapper>, EventListener
	{
	public:
		// 通过 IMGUIObject 继承
		void drawIMGUI(bool * isOpen) override;
		KeyMapper();
		void open(std::function<void(int)> keyChangeCallBack);
		bool isOpen();
		void hide();
		bool onKeyRelease(int keyCode) override;
    private:
		std::function<void(int)> m_changeCB {};
		bool m_isOpen = false;
	};

}
