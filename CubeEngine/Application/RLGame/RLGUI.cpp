#include "RLGUI.h"

#include "Engine/Engine.h"
#include "RLWorld.h"
#include "RLHero.h"
#include "RLPlayerState.h"
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
	ImGui::Text("HP : %d",int(hero->getHP()));
	ImGui::Text("Score : %u",RLPlayerState::shared()->getScore());
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

}

