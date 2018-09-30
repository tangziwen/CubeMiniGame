#ifndef TZW_MAINMENU_H
#define TZW_MAINMENU_H
#include "2D/GUISystem.h"
#include "Game/DebugInfoPanel.h"
#include "Event/Event.h"
#include "Engine/EngineDef.h"
namespace tzw {
class MainMenu : public IMGUIObject, public EventListener
{
public:
	TZW_SINGLETON_DECL(MainMenu);
    MainMenu();
	void init();
    void show();
    void hide();
    void toggle();
	void drawIMGUI() override;
	bool onKeyPress(int keyCode) override;

	bool isVisible() const;
	void setVisible(bool val);
private:
	bool m_isVisible = true;
    void startGame();
	void drawToolsMenu();
	void ShowExampleAppLog(bool* p_open);
	void ShowExampleAppConsole(bool* p_open);
	bool m_isShowProfiler;
	bool m_isShowConsole;
	DebugInfoPanel m_debugInfoPanel;
};

} // namespace tzw

#endif // TZW_MAINMENU_H
