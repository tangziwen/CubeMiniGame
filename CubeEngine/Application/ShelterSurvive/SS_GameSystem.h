#ifndef SS_GAME_SYSTEM_H
#define SS_GAME_SYSTEM_H
#include "Engine/EngineDef.h"
#include "SS_MainMenu.h"
#include "SS_Player.h"
namespace tzw
{
	class SS_GameSystem
	{
	public:
		void init();
		tzw::Node * getMainRoot() const;
		void setMainRoot(tzw::Node * val);
		tzw::SS_Player * getPlayer() const;
		void setPlayer(tzw::SS_Player * val);
		void startGame();
	private:
		SS_MainMenu * m_mainMenu;
		Node * m_mainRoot;
		SS_Player * m_player;
	private:
		TZW_SINGLETON_DECL(SS_GameSystem)
		
	};
}


#endif // !SS_GAME_SYSTEM_H
