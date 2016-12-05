#ifndef SS_MAIN_MENU
#define SS_MAIN_MENU
#include "EngineSrc/2D/GUITitledFrame.h"
#include "EngineSrc/2D/Button.h"
namespace tzw {

	class SS_MainMenu : public Node
	{
	public:
		SS_MainMenu();
		void show();
		void hide();
		void toggle();
	private:
		void startGame(Button *btn);
		GUIWindow * m_mainFrame;
		Button * m_buttons[5];
	};
} // namespace tzw
#endif
