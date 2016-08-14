#ifndef TZW_MAINMENU_H
#define TZW_MAINMENU_H
#include "EngineSrc/2D/GUITitledFrame.h"
#include "EngineSrc/2D/Button.h"
namespace tzw {

class MainMenu : public Node
{
public:
    MainMenu();
    void show();
    void hide();
    void toggle();
private:
    void startGame(Button *btn);
    GUIWindow * m_mainFrame;
    Button * m_buttons[5];
};

} // namespace tzw

#endif // TZW_MAINMENU_H
