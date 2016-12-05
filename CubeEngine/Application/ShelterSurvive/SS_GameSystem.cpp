#include "SS_GameSystem.h"
#include "Scene/SceneMgr.h"
namespace tzw
{
	TZW_SINGLETON_IMPL(SS_GameSystem)

	void SS_GameSystem::init()
	{
		m_mainMenu = new SS_MainMenu();
		g_GetCurrScene()->addNode(m_mainMenu);
		m_mainRoot = new Node();
		g_GetCurrScene()->addNode(m_mainRoot);
	}

	tzw::Node * SS_GameSystem::getMainRoot() const
	{
		return m_mainRoot;
	}

	void SS_GameSystem::setMainRoot(tzw::Node * val)
	{
		m_mainRoot = val;
	}

	tzw::SS_Player * SS_GameSystem::getPlayer() const
	{
		return m_player;
	}

	void SS_GameSystem::setPlayer(tzw::SS_Player * val)
	{
		m_player = val;
	}

	void SS_GameSystem::startGame()
	{
		m_player = new SS_Player(m_mainRoot);

		SkyBox* skybox = SkyBox::create("./Res/User/CubeGame/texture/SkyBox/right.jpg","./Res/User/CubeGame/texture/SkyBox/left.jpg",
			"./Res/User/CubeGame/texture/SkyBox/top.jpg","./Res/User/CubeGame/texture/SkyBox/bottom.jpg",
			"./Res/User/CubeGame/texture/SkyBox/back.jpg","./Res/User/CubeGame/texture/SkyBox/front.jpg");
		skybox->setScale(80,80,80);
		g_GetCurrScene()->setSkyBox(skybox);
	}

}