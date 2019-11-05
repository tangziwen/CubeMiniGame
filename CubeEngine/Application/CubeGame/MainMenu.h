#ifndef TZW_MAINMENU_H
#define TZW_MAINMENU_H
#include "2D/GUISystem.h"
#include "Game/DebugInfoPanel.h"
#include "Event/Event.h"
#include "Engine/EngineDef.h"
#include "GameNodeEditor.h"
#include "2D/Sprite.h"
#include "2D/GUIFileBrowser.h"
#include "2D/LabelNew.h"

namespace tzw {
class VehicleBroswer;
enum class WindowType
{
	INVENTORY,
	NODE_EDITOR,
	VEHICLE_FILE_BROWSER,
	RESUME_MENU,
	HELP_PAGE,
	ATTRIBUTE_WINDOW,
};
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
	void initInGame();
	GameNodeEditor * getNodeEditor();
	void setIsShowNodeEditor(bool isShow);
	void setIsShowAssetEditor(bool isShow);
	void popFloatTips(std::string floatString);
	void closeAllOpenedWindow();
	bool isNeedShowWindow();
	bool isAnyShow();
	void setIsFileBroswerOpen(bool isOpen);
	bool isOpenAssetEditor() const;
	void closeCurrentWindow();
	void setWindowShow(WindowType type, bool isShow);
	bool getWindowIsShow(WindowType type) const;
private:
	bool m_isVisible = true;
    void startGame();
	void drawToolsMenu();
	void ShowExampleAppLog(bool* p_open);
	void ShowExampleAppConsole(bool* p_open);
	
	bool m_isShowProfiler;
	bool m_isShowConsole;
	bool m_isOpenTerrain;
	bool m_isOpenRenderEditor;
	bool m_preIsNeedShow;
	DebugInfoPanel m_debugInfoPanel;
	GameNodeEditor * m_nodeEditor;
	VehicleBroswer * m_fileBrowser;
	Sprite * m_crossHair;
	LabelNew * m_crossHairTipsInfo;
public:
	LabelNew* getCrossHairTipsInfo() const;
};

} // namespace tzw

#endif // TZW_MAINMENU_H
