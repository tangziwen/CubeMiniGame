#include "RLGUI.h"

#include "Engine/Engine.h"
#include "RLWorld.h"
#include "RLHero.h"
#include "RLPlayerState.h"
#include "RLDirector.h"
#include "RLHeroCollection.h"
#include "2D/LabelNew.h"
#include "RLPerk.h"
#include "RLShopMgr.h"
namespace tzw
{
RLGUI::RLGUI()
{
}

void RLGUI::init()
{
	GUISystem::shared()->addObject(this);
	m_centerTips = LabelNew::create("HEHEHE");
	m_centerTips->setLocalPiority(999);
	vec2 size = Engine::shared()->winSize();
	m_centerTips->setPos2D(size.x / 2.f, size.y / 2.f);
	g_GetCurrScene()->addNode(m_centerTips);

}

void RLGUI::drawIMGUI()
{
	switch (RLWorld::shared()->getCurrGameState())
	{
	case RL_GameState::Playing:
		drawInGame();
		break;
	case RL_GameState::Pause:
		drawPause();
		break;
	case RL_GameState::Prepare:
		drawPrepareMenu();
		break;
	case RL_GameState::MainMenu:
		drawMainMenu();
	break;
	case RL_GameState::AfterMath:
		drawAfterMath();
	break;
	case RL_GameState::Win:
		drawWin();
	break;
	case RL_GameState::Purchase:
		drawPurchaseMenu();
	break;
	case RL_GameState::PerkSlect:
		drawPerkSelection();
	break;
	}

}

void RLGUI::drawMainMenu()
{
	
	GUISystem::shared()->imguiUseLargeFont();
	auto screenSize = Engine::shared()->winSize();
	ImGui::SetNextWindowPos(ImVec2(screenSize.x / 2.0, screenSize.y / 2.0), ImGuiCond_Always, ImVec2(0.5, 0.5));
	if (ImGui::Begin("RL Shooter",0, ImGuiWindowFlags_NoMove| ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse))
	{

		
		if(ImGui::Button(TRC("New Game"), ImVec2(160, 35)))
		{
			//setWindowShow(WindowType::NEW_WORLD_SETTING, true);
			RLWorld::shared()->goToPrepare();
		}
		ImGui::Spacing();


		if(ImGui::Button(TRC("Collections"), ImVec2(160, 35)))
		{
			RLWorld::shared()->goToPurchase();
		}
		ImGui::Spacing();
		if(ImGui::Button(TRC("Help"), ImVec2(160, 35)))
		{
			auto a = new Texture();
                  a->loadAsync(
                    "Texture/modern-tile1-ue/modern-tile1-albedo.png", 
					  [a](Texture * tex)
					{
						tlog("yes we finished");
						auto sprite = new Sprite();
						sprite->initWithTexture(tex);
						sprite->setContentSize(vec2(200, 200));
						g_GetCurrScene()->addNode(sprite);
					}
				  );
		}
		ImGui::Spacing();
		if(ImGui::Button(TRC("Exit"), ImVec2(160, 35))) 
		{
			RLWorld::shared()->endGame();
		}
		ImGui::TextColored(ImVec4(0.7f, 0.63f, 0.0f, 1.0f), "Gold: %u", RLPlayerState::shared()->getGold());
		ImGui::Spacing();
		ImGui::End();
		ImGui::PopFont();
	}
}

void RLGUI::drawInGame()
{
	vec2 screenSize = Engine::shared()->winSize();
	float yOffset = 15.0;
	ImVec2 window_pos = ImVec2(screenSize.x / 2.0, screenSize.y - yOffset);
	ImVec2 window_pos_pivot = ImVec2(0.5, 1.0);

	ImVec2 window_pos_pivot_bottom_right = ImVec2(1.0, 1.0);
	ImGui::SetNextWindowPos(ImVec2(50.0, screenSize.y - yOffset), ImGuiCond_Always, window_pos_pivot_bottom_right);
	ImGui::Begin("Rotate Tips", 0, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground| ImGuiWindowFlags_NoBringToFrontOnFocus);

	RLHero * hero =  RLWorld::shared()->getPlayerController()->getPossessHero();
	
	float progressBarWidth = screenSize.x - 55.f;
	ImGui::Text("Score : %u",RLPlayerState::shared()->getScore());

	if(RLDirector::shared()->isFinished())
	{
		ImGui::Text("Wave : Finished!!!");
	}
	else
	{
		if(RLDirector::shared()->getCurrentSubWave() == -1)
		{
			ImGui::Text("Wave : Prepare!!!");
		}
		else
		{
			ImGui::Text("Wave : %d - %d",RLDirector::shared()->getCurrentWave(), RLDirector::shared()->getCurrentSubWave());
		}
	}

	char tmp[64];



	

	ImGui::Text("Gold: %u", RLPlayerState::shared()->getGold());
	
	
	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(1.0, 1.0, 1.0, 0.5));

	sprintf(tmp, "%d / %d",  int(hero->getHP()), int(hero->getMAXHP()));
	ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.2, 0.81, 0.2, 0.5));
	ImGui::ProgressBar(hero->getHP()/ hero->getMAXHP(), ImVec2(progressBarWidth, 0), tmp);
	ImGui::PopStyleColor();


	ImGui::PushID("Mana Bar");

	sprintf(tmp, "%d / %d",  int(hero->getMana()), int(hero->getMaxMana()));
	ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.3, 0.41, 1.0, 0.5));

	ImGui::ProgressBar(hero->getMana() / hero->getMaxMana(), ImVec2(progressBarWidth, 0), tmp);
	ImGui::PopStyleColor();
	ImGui::PopID();

	sprintf(tmp, "Lv: %u",  RLPlayerState::shared()->getCurrLevel());
	ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.5, 0.95, 1.0, 0.5));
	float levelRatio = (RLPlayerState::shared()->getCurrExp() * 1.f) / RLPlayerState::shared()->getMaxExp();
	ImGui::PushID("Level Bar");
	ImGui::ProgressBar(levelRatio, ImVec2(progressBarWidth, 0), tmp);
	ImGui::PopID();
	ImGui::PopStyleColor();

	ImGui::PopStyleColor();
	




	ImGui::End();



	ImGui::SetNextWindowPos(ImVec2(50.0, 0.0), ImGuiCond_Always, window_pos_pivot_bottom_right);
	ImGui::Begin("HUD", 0, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground| ImGuiWindowFlags_NoBringToFrontOnFocus);
	ImGui::SetWindowFontScale(5.0f);
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.f, 1.0f, 1.0f));
	ImGui::Text("Waves: %d", RLDirector::shared()->getCurrentSubWave() + 1);
	RLWeapon * weapon = hero->getWeapon();
	if(hero->getWeapon())
	{
		if(weapon->isReloading())
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.647f, 0.0f, 1.0f));
			ImGui::Text("Reloading.");
			ImGui::PopStyleColor();
		}
		else
		{
			if(weapon->getCurrRound()== 0)
			{
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
				ImGui::Text("AMMO: Empty");
				ImGui::PopStyleColor();
			}
			else
			{
				ImGui::Text("AMMO: %d / %d", hero->getWeapon()->getCurrRound(), hero->getWeapon()->getTotalRound());
			}
		}
	}
	ImGui::PopStyleColor();
	ImGui::End();

}

void RLGUI::drawAfterMath()
{
	GUISystem::shared()->imguiUseLargeFont();
	auto screenSize = Engine::shared()->winSize();
	ImGui::SetNextWindowPos(ImVec2(screenSize.x / 2.0, screenSize.y / 2.0), ImGuiCond_Always, ImVec2(0.5, 0.5));
	if (ImGui::Begin("RL Shooter",0, ImGuiWindowFlags_NoMove| ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse))
	{

		ImGui::Text("your Score : %u",RLPlayerState::shared()->getScore());
		if(ImGui::Button(TRC("To Main Menu"), ImVec2(160, 35)))
		{
			RLWorld::shared()->goToMainMenu();
		}
		ImGui::Spacing();
		if(ImGui::Button(TRC("Exit To Windows"), ImVec2(160, 35))) 
		{
			RLWorld::shared()->endGame();
		}
		ImGui::Spacing();
		ImGui::End();
		ImGui::PopFont();
	}
}

void RLGUI::drawWin()
{
	GUISystem::shared()->imguiUseLargeFont();
	auto screenSize = Engine::shared()->winSize();
	ImGui::SetNextWindowPos(ImVec2(screenSize.x / 2.0, screenSize.y / 2.0), ImGuiCond_Always, ImVec2(0.5, 0.5));
	if (ImGui::Begin("Congrats!!!",0, ImGuiWindowFlags_NoMove| ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse))
	{
		ImGui::Text("Winner!!");
		ImGui::Text("Your Score : %u",RLPlayerState::shared()->getScore());
		if(ImGui::Button(TRC("Back To Menu"), ImVec2(160, 35)))
		{
			RLWorld::shared()->goToMainMenu();
		}
		ImGui::Spacing();
		if(ImGui::Button(TRC("Exit To Windows"), ImVec2(160, 35))) 
		{
			RLWorld::shared()->endGame();
		}
		ImGui::Spacing();
		ImGui::End();
	}
	ImGui::PopFont();
}

void RLGUI::drawPause()
{
	GUISystem::shared()->imguiUseLargeFont();
	auto screenSize = Engine::shared()->winSize();
	ImGui::SetNextWindowPos(ImVec2(screenSize.x / 2.0, screenSize.y / 2.0), ImGuiCond_Always, ImVec2(0.5, 0.5));
	if (ImGui::Begin("Pause Windows",0, ImGuiWindowFlags_NoMove| ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse))
	{
		ImGui::Text("Pause");
		if(ImGui::Button(TRC("Resume"), ImVec2(160, 35)))
		{
			RLWorld::shared()->resumeGame();
		}
		if(ImGui::Button(TRC("Back To Menu"), ImVec2(160, 35)))
		{
			RLWorld::shared()->goToMainMenu();
		}
		ImGui::Spacing();
		if(ImGui::Button(TRC("Exit To Windows"), ImVec2(160, 35))) 
		{
			RLWorld::shared()->endGame();
		}
		ImGui::Spacing();
		ImGui::End();
	}
	ImGui::PopFont();
}

void RLGUI::drawPurchaseMenu()
{
	auto screenSize = Engine::shared()->winSize();
	ImGui::SetNextWindowPos(ImVec2(screenSize.x / 2.0, screenSize.y / 2.0), ImGuiCond_Always, ImVec2(0.5, 0.5));
	if (ImGui::Begin("Purchase!!!",0, ImGuiWindowFlags_NoMove| ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize  | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse))
	{
		ImGui::Text("Purchase your hero, Gold: %u", RLPlayerState::shared()->getGold());
		ImGui::BeginChild("InventoryViewer",ImVec2(int(350), 300), true);
		ImGui::Columns(3,"InventoryCol",false);
		auto & heroList = RLHeroCollection::shared()->getPlayableHeroDatas();
		for(int i = 0; i < heroList.size(); i++)
		{
			ImGui::PushID(i);
			ImGui::BeginGroup();
			ImGui::Text(" %s", heroList[i].m_name.c_str());
			if(RLPlayerState::shared()->isHeroUnLock(heroList[i].m_name))
			{
				int level = RLPlayerState::shared()->getHeroLevel(heroList[i].m_name);
				if(level < 5)
				{
					ImGui::SameLine();
					if(RLPlayerState::shared()->isCanAffordHeroPurchase(heroList[i].m_name, HeroPurchaseAction::Unlock))
					{
						if(ImGui::Button("+(100)"))
						{
							RLPlayerState::shared()->heroUpdate(heroList[i].m_name);
						}
					
					}
					else
					{
						ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetColorU32(ImGuiCol_TextDisabled));
						ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::GetColorU32(ImGuiCol_TextDisabled));
						ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImGui::GetColorU32(ImGuiCol_TextDisabled));
						
						
						ImGui::Button("+(100)");
						ImGui::PopStyleColor();
						ImGui::PopStyleColor();
						ImGui::PopStyleColor();
					}




					ImGui::Text("Lv:%d", level);
				}
				else
				{
					ImGui::Text("Lv:5(Max)");
				}

				
			}
			else
			{
				if(RLPlayerState::shared()->isCanAffordHeroPurchase(heroList[i].m_name, HeroPurchaseAction::Unlock))
				{
					if(ImGui::Button("Buy(500)"))
					{
						RLPlayerState::shared()->unlockHero(heroList[i].m_name);
					}
				}
				else
				{
					

					ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetColorU32(ImGuiCol_TextDisabled));
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::GetColorU32(ImGuiCol_TextDisabled));
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImGui::GetColorU32(ImGuiCol_TextDisabled));
					ImGui::Button("Buy(500)");
					ImGui::PopStyleColor();
					ImGui::PopStyleColor();
					ImGui::PopStyleColor();
				
				}

			}
			
			ImGui::EndGroup();
			ImGui::Spacing();
			ImGui::PopID();
			ImGui::NextColumn();
		}
		ImGui::EndChild();
		ImGui::SameLine(0, -1);
		ImGui::BeginChild("RightPanel",ImVec2(int(90), 0), 0);
		if(ImGui::Button(TRC("Back")))
		{
			RLWorld::shared()->goToMainMenu();
		}
		ImGui::EndChild();
		ImGui::End();
	}
}

void RLGUI::drawPrepareMenu()
{
	auto screenSize = Engine::shared()->winSize();
	ImGui::SetNextWindowPos(ImVec2(screenSize.x / 2.0, screenSize.y / 2.0), ImGuiCond_Always, ImVec2(0.5, 0.5));
	if (ImGui::Begin("Purchase!!!",0, ImGuiWindowFlags_NoMove| ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize  | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse))
	{
		ImGui::Text("Gold: %u", RLPlayerState::shared()->getGold());
		ImGui::BeginChild("InventoryViewer",ImVec2(int(400), 400), true);
		ImGui::Columns(3,"InventoryCol",false);
		static int currselectIdx = 0;
		auto & heroList = RLHeroCollection::shared()->getPlayableHeroDatas();
		for(int i = 0; i < heroList.size(); i++)
		{
			
			ImGui::PushID(i);
			ImGui::BeginGroup();
			if(RLPlayerState::shared()->isHeroUnLock(heroList[i].m_name))
			{
				int level = RLPlayerState::shared()->getHeroLevel(heroList[i].m_name);
				char tmpStr[128];
				sprintf(tmpStr, "%s(%d)",heroList[i].m_name.c_str(), level);
				if(ImGui::RadioButton( tmpStr, currselectIdx == i))
				{
					currselectIdx = i;
				}
			}
			else
			{
				ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetColorU32(ImGuiCol_TextDisabled));
				ImGui::RadioButton( "?????", 0);
				ImGui::PopStyleColor();
			}

			ImGui::EndGroup();
			ImGui::Spacing();
			ImGui::PopID();
			ImGui::NextColumn();
		}
		ImGui::EndChild();
		ImGui::SameLine(0, -1);
		ImGui::BeginChild("Start Game",ImVec2(int(90), 0), true);
		if(ImGui::Button(TRC("Start")))
		{
			RLWorld::shared()->startGame(heroList[currselectIdx].m_name);
		}
		if(ImGui::Button(TRC("Back")))
		{
			RLWorld::shared()->goToMainMenu();
		}
		ImGui::EndChild();
		ImGui::End();
	}
}

void RLGUI::drawPerkSelection()
{
	RLShopMgr::shared()->draw();
}

}

