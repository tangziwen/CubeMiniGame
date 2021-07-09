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
#include "Utility/file/Data.h"
#include "OptionPanel.h"
namespace tzw {
class PainterUI;
class VehicleBroswer;
class NewWorldSettingUI;
class LoadWorldUI;
enum class WindowType
{
	INVENTORY,
	NODE_EDITOR,
	VEHICLE_FILE_BROWSER,
	RESUME_MENU,
	HELP_PAGE,
	ATTRIBUTE_WINDOW,
	PAINTER,
	MainMenu,
	OPTION_MENU,
	QUICK_DEBUG,
	WORLD_SETTING,
	PLAYER_INFO,
	ABOUT,
	Console,
	NEW_WORLD_SETTING,
	LOAD_WORLD,
};
class GameUISystem : public Singleton<GameUISystem>, public IMGUIObject, public EventListener
{
public:
    GameUISystem();
	void init();
    void show();
    void hide();
    void toggle();
	void drawIMGUI() override;
	bool onKeyPress(int keyCode) override;

	bool isVisible() const;
	void setVisible(bool val);
	void initInGame();
	void openNodeEditor(GamePart * part);
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
	void openInspectWindow(GamePart * part);
	void setPainterShow(bool isShow);
	void drawEntryInterFace();
	void openMainMenu();
	void setIsNeedShowCrossHair(bool isNeedShow);
private:
	bool m_isVisible = true;
	void drawToolsMenu();
	void ShowExampleAppLog(bool* p_open);
	void ShowExampleAppConsole(bool* p_open);
	void drawInventory();
	
	bool m_isShowProfiler;
	bool m_isShowConsole;
	bool m_isOpenTerrain;
	bool m_isOpenRenderEditor;
	bool m_preIsNeedShow;
	bool m_isOpenPlayerOverLay;
	bool m_isNeedShowCrossHair {true};
	DebugInfoPanel m_debugInfoPanel;
	VehicleBroswer * m_fileBrowser;
	NewWorldSettingUI * m_NewWorldSettingUI;
	LoadWorldUI * m_loadWorldUI;
	PainterUI * m_painterUI;
	Sprite * m_crossHair;
	LabelNew * m_crossHairTipsInfo;
	GamePart * m_curInspectPart;
	Data m_helperData;
	OptionPanel m_option;
	GamePart* m_currControlPart;
public:
	LabelNew* getCrossHairTipsInfo() const;
};

} // namespace tzw

#endif // TZW_MAINMENU_H
