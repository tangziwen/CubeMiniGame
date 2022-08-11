#include "RLGUI.h"

#include "Engine/Engine.h"
#include "RLWorld.h"
#include "RLHero.h"
#include "RLPlayerState.h"
#include "RLDirector.h"
namespace tzw
{
RLGUI::RLGUI()
{
}

void RLGUI::init()
{
	GUISystem::shared()->addObject(this);
}

void RLGUI::drawIMGUI()
{
	switch (RLWorld::shared()->getCurrGameState())
	{
	case RL_GameState::Playing:
		drawInGame();
		break;
	case RL_GameState::MainMenu:
		drawMainMenu();
	break;
	case RL_GameState::AfterMath:
		drawAfterMath();
	break;
	case RL_GameState::Win:
		drawAfterMath();
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
			RLWorld::shared()->startGame();
		}
		ImGui::Spacing();


		if(ImGui::Button(TRC("Option"), ImVec2(160, 35)))
		{

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
			TranslationMgr::shared()->dump();
			exit(0);
		}
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
	ImGui::Begin("Rotate Tips", 0, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus);

	RLHero * hero =  RLWorld::shared()->getPlayerController()->getPossessHero();

	float progressBarWidth = 150.f;
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
	sprintf(tmp, "%d / %d",  int(hero->getHP()), int(hero->getMAXHP()));
	ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.2, 0.81, 0.2, 1.0));
	ImGui::ProgressBar(hero->getHP()/ hero->getMAXHP(), ImVec2(progressBarWidth, 0), tmp);
	ImGui::PopStyleColor();


	ImGui::PushID("Mana Bar");

	sprintf(tmp, "%d / %d",  int(hero->getMana()), int(hero->getMaxMana()));
	ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.3, 0.41, 1.0, 1.0));

	ImGui::ProgressBar(hero->getMana() / hero->getMaxMana(), ImVec2(progressBarWidth, 0), tmp);
	ImGui::PopStyleColor();
	ImGui::PopID();


	


	sprintf(tmp, "Lv: %u",  RLPlayerState::shared()->getCurrLevel());
	ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.5, 0.95, 1.0, 1.0));
	float levelRatio = (RLPlayerState::shared()->getCurrExp() * 1.f) / RLPlayerState::shared()->getMaxExp();
	ImGui::PushID("Level Bar");
	ImGui::ProgressBar(levelRatio, ImVec2(progressBarWidth, 0), tmp);
	ImGui::PopID();
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
		if(ImGui::Button(TRC("Retry"), ImVec2(160, 35)))
		{
			//setWindowShow(WindowType::NEW_WORLD_SETTING, true);
			RLWorld::shared()->startGame();
		}
		ImGui::Spacing();
		if(ImGui::Button(TRC("To Main Menu"), ImVec2(160, 35)))
		{
			RLWorld::shared()->goToMainMenu();
		}
		ImGui::Spacing();
		if(ImGui::Button(TRC("Exit To Windows"), ImVec2(160, 35))) 
		{
			exit(0);
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
		ImGui::Text("Your Score : %u",RLPlayerState::shared()->getScore());
		if(ImGui::Button(TRC("Back To Menu"), ImVec2(160, 35)))
		{
			RLWorld::shared()->goToMainMenu();
		}
		ImGui::Spacing();
		if(ImGui::Button(TRC("Exit To Windows"), ImVec2(160, 35))) 
		{
			exit(0);
		}
		ImGui::Spacing();
		ImGui::End();
	}
	ImGui::PopFont();
}

}

