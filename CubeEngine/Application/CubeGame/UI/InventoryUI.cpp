#include "InventoryUI.h"
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
	void InventoryUI::drawIMGUI(bool* isOpen)
	{
	*isOpen = ImGui::Begin("AssetWindow", 0);
	ImGui::SetWindowSize(ImVec2(300, 0), ImGuiCond_Once);
	int i = 0;
	int itemSize = 80;
	int realItemSize = itemSize;
	m_isDragingInventory = false;

	float window_visible_x2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
	float last_button_x2 = 0;

	int inventoryAmount = GameItemMgr::shared()->getItemAmount();
	auto &s = ImGui::GetStyle();
	float spaceX = s.ItemSpacing.x;
	float padding = s.FramePadding.x;
	float rightPanelWidth = 150;
	float size = ImGui::GetWindowWidth() - s.IndentSpacing - rightPanelWidth;
	ImGui::BeginChild("InventoryViewer",ImVec2(int(size), 0), 0);
	
	ImGui::Columns(std::floor(size / (itemSize + (2 * padding))),"InventoryCol",false);
	//for k, v in pairs(GameState.m_inventory) :
	for (i =0 ; i < inventoryAmount; i++)
	{

		ImGui::PushID(i);
		ImGui::BeginGroup();
		auto iconTexture = m_emptyIcon->handle();
		auto item = GameItemMgr::shared()->getItemByIndex(i);
		if (item->getThumbNailTextureId() != nullptr)
		{
			iconTexture = item->getThumbNailTextureId();
		}
			


		if(ImGui::ImageButton(iconTexture, ImVec2(itemSize, itemSize)))
		{
			m_currentSelectItem  = item;
		}
			
		

		//print("aaaaaaaaaaaa"..GameState.currentSelectedItem.m_desc)
		float last_button_x2 = ImGui::GetItemRectMax().x - ImGui::GetItemRectMin().x;
		float next_button_x2 = last_button_x2 + s.ItemSpacing.x + itemSize; //Expected position if next button was on same line

		//Our buttons are both drag sources and drag targets here!
		if (ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload("DND_DEMO_CELL", item, sizeof(item));    //# Set payload to carry the index of our item (could be anything)
			ImGui::Text("Dragging");   // Display preview (could be anything, e.g. when dragging an image we could decide to display the filename and a small preview of the image, etc.)
			ImGui::EndDragDropSource();
			m_isDragingInventory = true;
		}

		
		if (m_currentSelectItem && m_currentSelectItem->m_name == item->m_name)
		{
			ImVec2 sizeMin = ImGui::GetItemRectMin();
			ImVec2 sizeMax = ImGui::GetItemRectMax();
			imgui_drawFrame(sizeMin, sizeMax, 2, ImVec4(0, 0, 0, 1));
		}

		
		ImGui::Text("%s", item->m_desc);

		ImGui::EndGroup();
		ImGui::PopID();

		ImGui::NextColumn();
	}

	
	ImGui::Columns(1,"InventoryCol",false);
	ImGui::EndChild();
	ImGui::SameLine(0, -1);
	ImGui::BeginChild("DetailInfo",ImVec2(0, 0), 0);
	if (m_currentSelectItem)
	{
		ImGui::Text(m_currentSelectItem->m_desc.c_str());
		ImGui::TextWrapped(m_currentSelectItem->m_description.c_str());
		//ImGui::TextWrapped(CubeEngine.TR("Mass: ")+ str(GameState.currentSelectedItem.m_physicsInfo.mass));
		vec3 size = m_currentSelectItem->m_physicsInfo.size;
		ImGui::TextWrapped("Size %fX%fX%f", size.x, size.y, size.z);
	}

	
	ImGui::EndChild();
	ImGui::End();
	}
	InventoryUI::InventoryUI():m_onCreate(nullptr)
	{
		m_emptyIcon = TextureMgr::shared()->getByPathSimple("UITexture/Icon/icons8-border-none-96.png");
	}

	void InventoryUI::prepare()
	{
		
	}

}