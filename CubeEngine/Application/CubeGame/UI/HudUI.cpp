#include "HudUI.h"
#include <filesystem>


#include "CubeGame/GameItemMgr.h"
#include "CubeGame/PartSurfaceMgr.h"
#include "Rendering/GraphicsRenderer.h"
#include "2D/imgui_internal.h"
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
		/*
		screenSize = CubeEngine.Engine.shared().winSize()
		yOffset = 15.0;
		window_pos = ImGui.ImVec2(screenSize.x / 2.0, screenSize.y - yOffset);
		window_pos_pivot = ImGui.ImVec2(0.5, 1.0);

		window_pos_pivot_bottom_right = ImGui.ImVec2(1.0, 1.0);
		ImGui.SetNextWindowPos(ImGui.ImVec2(screenSize.x - 50.0, screenSize.y - yOffset), ImGuiCond_Always, window_pos_pivot_bottom_right);
		ImGui.Begin("Rotate Tips", ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus);
		if Game.BuildingSystem.shared().isIsInXRayMode() :
			ImGui.Text(CubeEngine.TR("X光模式"));
		else:
			ImGui.Text(CubeEngine.TR("当前旋转角度")+ str(GameState.g_blockRotate));
	
		ImGui.End()


		ImGui.SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
		itemSize = 80.0
		ImGui.Begin("Hud", ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);
		selectColor = ImGui.ImVec4(1.0, 0.5, 0.1, 1)
		for k, v in GameState.m_itemSlots.items():
			# ImGui.RadioButton(v.desc, GameState.m_currIndex == k)
			ImGui.BeginGroup();
			needPop = False
			if v.target == None :
				indexText = str(k + 1)
				posX = ImGui.GetCursorPosX()
				CubeEngine.GUISystem.shared().imguiUseLargeFont()
				textSize = ImGui.CalcTextSize(indexText, None, True, 0);
				ImGui.SetCursorPosX(posX + itemSize / 2.0 - textSize.x / 2.0)
				ImGui.Text(indexText);
				ImGui.PopFont()
				ImGui.Image(GameState.testIcon.handle(), ImGui.ImVec2(itemSize, itemSize));
				if GameState.m_currIndex == k :
					ImGui.PushStyleColor(0, selectColor);
					sizeMin = ImGui.GetItemRectMin()
					sizeMax = ImGui.GetItemRectMax()
				
					ImGui.DrawFrame(sizeMin, sizeMax, 3.0, selectColor)
					needPop = True
				posX = ImGui.GetCursorPosX()
				textSize = ImGui.CalcTextSize("Empty", None, True, 0);
				ImGui.SetCursorPosX(posX + itemSize / 2.0 - textSize.x / 2.0)
				ImGui.Text("Empty")
			else:
			
				iconTexture = GameState.testIcon.handle()
				item = getItemFromSpecifiedSlotIndex(k)
				indexText = str(k + 1)
				posX = ImGui.GetCursorPosX()
				CubeEngine.GUISystem.shared().imguiUseLargeFont()
				textSize = ImGui.CalcTextSize(indexText, None, True, 0);
				ImGui.SetCursorPosX(posX + itemSize / 2.0 - textSize.x / 2.0)
				ImGui.Text(indexText)
				ImGui.PopFont()
				if item.getThumbNailTextureId() != 0 :
					iconTexture = item.getThumbNailTextureId()
				ImGui.Image(iconTexture, ImGui.ImVec2(itemSize, itemSize));
			
				if GameState.m_currIndex == k :
					ImGui.PushStyleColor(0, selectColor);
					needPop = True
					sizeMin = ImGui.GetItemRectMin()
					sizeMax = ImGui.GetItemRectMax()
					ImGui.DrawFrame(sizeMin, sizeMax, 3.0, selectColor)
				posX = ImGui.GetCursorPosX()
				textSize = ImGui.CalcTextSize(item.m_desc, None, True, 0);
				ImGui.SetCursorPosX(posX + itemSize / 2.0 - textSize.x / 2.0)
				ImGui.Text(item.m_desc)
			if needPop :
				ImGui.PopStyleColor()
		
			ImGui.EndGroup()
			if ImGui.BeginDragDropTargetAnyWindow() :
				payLoad = ImGui.AcceptDragDropPayload("DND_DEMO_CELL", 0)
				# print("payLoad", payLoad)
				if payLoad != None :
					payLoadIdx = ImGui.GetPayLoadData2Int(payLoad)
					payLoadItem = Game.GameItemMgr.shared().getItemByIndex(payLoadIdx)
					print ("Play load Here   "+ (payLoadItem.m_name))
					GameState.m_itemSlots[k].target = payLoadItem.m_name
					player = Game.GameWorld.shared().getPlayer()
					if k == GameState.m_currIndex :
						if GameState.m_itemSlots[GameState.m_currIndex].target:
							player.setCurrSelected(GameState.m_itemSlots[GameState.m_currIndex].target)
						else:
							player.setCurrSelected(None)
				
			
				ImGui.EndDragDropTarget()
		
			ImGui.SameLine(0, -1.0)
	
		ImGui.End()
		*/
	}

	HudUI::HudUI():m_onCreate(nullptr)
	{
		m_emptyIcon = TextureMgr::shared()->getByPathSimple("UITexture/Icon/icons8-border-none-96.png");
	}

	void HudUI::prepare()
	{
		
	}

}