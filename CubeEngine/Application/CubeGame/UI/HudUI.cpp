#include "HudUI.h"
#include <filesystem>


#include "CubeGame/GameItemMgr.h"
#include "CubeGame/PartSurfaceMgr.h"
#include "Rendering/GraphicsRenderer.h"
#include "2D/imgui_internal.h"
#include "CubeGame/BuildingSystem.h"
#include "2D/GUISystem.h"
#include <cstdlib>
namespace fs  = std::filesystem;
namespace tzw
{


	static void imgui_drawFrame(ImVec2 min, ImVec2 max, float size, ImVec4 color)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, size);
		ImGui::PushStyleColor(ImGuiCol_Border, color);
		ImGui::RenderFrameBorder(min, max, 1);
		ImGui::PopStyleVar(1);
		ImGui::PopStyleColor(1);
	}

	void HudUI::drawIMGUI(bool* isOpen)
	{

		vec2 screenSize = Engine::shared()->winSize();
		float yOffset = 15.0;
		ImVec2 window_pos = ImVec2(screenSize.x / 2.0, screenSize.y - yOffset);
		ImVec2 window_pos_pivot = ImVec2(0.5, 1.0);

		ImVec2 window_pos_pivot_bottom_right = ImVec2(1.0, 1.0);
		ImGui::SetNextWindowPos(ImVec2(screenSize.x - 50.0, screenSize.y - yOffset), ImGuiCond_Always, window_pos_pivot_bottom_right);
		ImGui::Begin("Rotate Tips", 0, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus);
		if (BuildingSystem::shared()->isIsInXRayMode())
		{
			ImGui::Text("XRay Mode");
		}
		else
		{
			ImGui::Text("Block Rotate");
			//ImGui::Text(CubeEngine.TR("当前旋转角度")+ str(GameState.g_blockRotate));
		}
		ImGui::End();


		ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
		float itemSize = 80.0;
		ImGui::Begin("Hud", 0, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);
		ImVec4 selectColor = ImVec4(1.0, 0.5, 0.1, 1);
		//for k, v in GameState.m_itemSlots.items():
		for(int k = 0; k < m_itemSlot.size(); k++)
		{
			GameItem * item = m_itemSlot[k];
			ImGui::BeginGroup();
			bool needPop = false;
			if (!item)
			{
				std::string indexText = std::to_string(k + 1);
				float posX = ImGui::GetCursorPosX();
				GUISystem::shared()->imguiUseLargeFont();
				ImVec2 textSize = ImGui::CalcTextSize(indexText.c_str(), 0, true, 0);
				ImGui::SetCursorPosX(posX + itemSize / 2.0 - textSize.x / 2.0);
				ImGui::Text(indexText.c_str());
				ImGui::PopFont();
				ImGui::Image(m_emptyIcon->handle(), ImVec2(itemSize, itemSize));
				if (m_currIndex == k)
				{
					ImGui::PushStyleColor(0, selectColor);
					ImVec2 sizeMin = ImGui::GetItemRectMin();
					ImVec2 sizeMax = ImGui::GetItemRectMax();
				
					imgui_drawFrame(sizeMin, sizeMax, 3.0, selectColor);
					needPop = true;
				}

				posX = ImGui::GetCursorPosX();
				textSize = ImGui::CalcTextSize("Empty", 0, true, 0);
				ImGui::SetCursorPosX(posX + itemSize / 2.0 - textSize.x / 2.0);
				ImGui::Text("Empty");
			}
			else
			{
				auto iconTexture = m_emptyIcon->handle();
				//item = getItemFromSpecifiedSlotIndex(k)

				std::string indexText = std::to_string(k + 1);
				float posX = ImGui::GetCursorPosX();
				GUISystem::shared()->imguiUseLargeFont();
				ImVec2 textSize = ImGui::CalcTextSize(indexText.c_str(), 0, true, 0);
				ImGui::SetCursorPosX(posX + itemSize / 2.0 - textSize.x / 2.0);
				ImGui::Text(indexText.c_str());
				ImGui::PopFont();
				if (item->getThumbNailTextureId() != 0)
				{
					iconTexture = item->getThumbNailTextureId();
				}
					
				ImGui::Image(iconTexture, ImVec2(itemSize, itemSize));
			
				if (m_currIndex == k)
				{
					ImGui::PushStyleColor(0, selectColor);
					needPop = true;
					ImVec2 sizeMin = ImGui::GetItemRectMin();
					ImVec2 sizeMax = ImGui::GetItemRectMax();
					imgui_drawFrame(sizeMin, sizeMax, 3.0, selectColor);
				}

				posX = ImGui::GetCursorPosX();
				textSize = ImGui::CalcTextSize(item->m_desc.c_str(), 0, true, 0);
				ImGui::SetCursorPosX(posX + itemSize / 2.0 - textSize.x / 2.0);
				ImGui::Text(item->m_desc.c_str());
			}
			
				
			if (needPop)
				ImGui::PopStyleColor();
		
			ImGui::EndGroup();
			if (ImGui::BeginDragDropTargetAnyWindow())
			{
				auto payLoad = ImGui::AcceptDragDropPayload("DND_DEMO_CELL", 0);

				if (payLoad)
				{
					GameItem* payLoadItem = nullptr;
					memcpy(&payLoadItem, payLoad->Data, sizeof(GameItem *));
					//payLoadItem = *(GameItem**)payLoad->Data;
					m_itemSlot[k] = payLoadItem;
					auto player = GameWorld::shared()->getPlayer();
					if (k == m_currIndex)
					{
						if (m_itemSlot[m_currIndex])
						{
							player->setCurrSelected(payLoadItem->m_name);
						}	
						else
						{
							player->setCurrSelected("");
						}
					}

				}
				ImGui::EndDragDropTarget();
			}

		
			ImGui::SameLine(0, -1.0);
		}
			
	
		ImGui::End();
	}

	HudUI::HudUI():m_onCreate(nullptr)
	{
		m_emptyIcon = TextureMgr::shared()->getByPathSimple("UITexture/Icon/icons8-border-none-96.png");
	}

	void HudUI::prepare()
	{
		
	}

}