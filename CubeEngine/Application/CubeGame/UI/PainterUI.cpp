#include "PainterUI.h"
#include <filesystem>


#include "CubeGame/GameItemMgr.h"
#include "CubeGame/PartSurfaceMgr.h"
#include "Rendering/GraphicsRenderer.h"
namespace fs  = std::filesystem;
namespace tzw
{
	void PainterUI::drawIMGUI(bool* isOpen)
	{
		auto screenSize = Engine::shared()->winSize();
		ImGui::SetNextWindowPos(ImVec2(screenSize.x / 2.0, screenSize.y / 2.0), ImGuiCond_Appearing, ImVec2(0.5, 0.5));
		ImGui::Begin(u8"Painter",isOpen, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);
        if (ImGui::BeginTabBar("Block Color", ImGuiTabItemFlags_None))
        {
			if (ImGui::BeginTabItem(TRC(u8"Block Surface"), 0, ImGuiTabItemFlags_None))
			{

				if(ImGui::TreeNode("Color Setting"))
				{

					ImGui::TextUnformatted(TRC(u8"Preset Colors:"));
					float factorList[] = {1.0, 0.9, 0.8, 0.6, 0.5, 0.3};
					float whiteFactorList[] = {1.0, 0.8, 0.6, 0.45, 0.2, 0.0};
					vec3 colorList[] = {vec3(1, 1, 1), vec3(1, 0, 0), vec3(0, 1, 0), vec3(0, 0, 1),
						vec3(1, 1, 0), vec3(1, 0, 1), vec3(0, 1, 1),
						vec3(1, 0.7, 0), vec3(0.7, 1.0, 0), vec3(1.0, 0.0, 0.7), vec3(0.7, 0.0, 1.0), vec3(0, 0.7, 1), vec3(0, 1, 0.7),
						vec3(1, 0.752, 0.796),
					};
					int id = 0;
					for(int i = 0; i < sizeof(colorList) / sizeof(vec3); i++)
					{
						auto color = colorList[i];
						for(int j = 0; j < 6; j ++)
						{
							ImGui::PushID(id);
							vec3 resultColor = color * factorList[j];
							if(i == 0) //special factor list for white
							{
								resultColor = color * whiteFactorList[j];
							}
							if(ImGui::ColorButton("##", ImVec4(resultColor.x, resultColor.y, resultColor.z, 1)))
							{
								GameWorld::shared()->getPlayer()->getPaintGun()->color = resultColor;
							}
							ImGui::SameLine();
							ImGui::PopID();
							++id;
						}
						ImGui::NewLine();
					}
					ImGui::NewLine();

					ImGui::TextUnformatted(TRC(u8"Custom Colors:"));
					auto customFlag = ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoOptions;
					for(int i =0 ; i < 8; i ++)
					{
						char tmpChar[50];
						sprintf(tmpChar, "CustomColor%d", i);
						if(ImGui::ColorButton(tmpChar, customColorList[i], customFlag))
						{
							GameWorld::shared()->getPlayer()->getPaintGun()->color = vec3(customColorList[i].x, customColorList[i].y, customColorList[i].z);
						}
						if(ImGui::IsItemClicked(1))
						{
							m_tmpColor = customColorList[i];
							m_tmpModifedColor = (float *)&customColorList[i].x;
							ImGui::OpenPopup("picker");
						}
						if(i == 4 - 1)
						{
							ImGui::NewLine();
						}else
						{
							ImGui::SameLine();
						}
					}
			        if (ImGui::BeginPopup("picker"))
			        {
						ImGui::ColorPicker4("##picker", m_tmpModifedColor, 0);
			            ImGui::EndPopup();
			        }
					ImGui::NewLine();
					auto col3 = GameWorld::shared()->getPlayer()->getPaintGun()->color;
					auto imCol4 = ImVec4(col3.x, col3.y, col3.z, 1.0f);
					ImGui::TextUnformatted(TRC(u8"当前颜色:"));
					ImGui::ColorEdit3("Current Color", (float *)&imCol4, ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_NoInputs);
					GameWorld::shared()->getPlayer()->getPaintGun()->color = vec3(imCol4.x, imCol4.y, imCol4.z);

					ImGui::TreePop();
				}
				if(ImGui::TreeNode("Surface Setting"))
				{
					auto p = GameWorld::shared()->getPlayer()->getPaintGun();
					//表面材质
					ImGui::TextUnformatted(TRC(u8"表面材质"));
					auto size = PartSurfaceMgr::shared()->getItemAmount();
					if(ImGui::RadioButton("Enable Surface Edit", p->m_enableSurfaceChanged))
					{
						p->m_enableSurfaceChanged = !p->m_enableSurfaceChanged;
					}
					ImGui::NewLine();
					ImGui::NewLine();
					ImGui::TextUnformatted(TRC(u8"Surface List:"));
					for(int i = 0; i < size; i++)
					{
						auto surface = PartSurfaceMgr::shared()->getItemByIndex(i);
	
						if(ImGui::RadioButton(surface->getName().c_str(), surface ==p->m_surface))
						{
							p->m_surface = surface;
						}
						if(i%2 == 0)
						{
							ImGui::SameLine();
						}
					}
					ImGui::TreePop();
				}
				if(ImGui::TreeNode("Make Block Template"))
				{
					static char newItemName[128] = "new Item";
					static char newItemTitle[128] = "new Item";
					ImGui::Text(TRC("you can set a template for block for further used"));
					auto& itemList = GameItemMgr::shared()->getItemList();
					static int currSelectedItemIndex = GameItemMgr::shared()->getItemIndex(GameItemMgr::shared()->getItem("Block"));
			        if (ImGui::BeginCombo(TRC(u8"Block List"), itemList[currSelectedItemIndex]->m_desc.c_str(), 0)) // The second parameter is the label previewed before opening the combo.
			        {
			            for (int n = 0; n < itemList.size(); n++)
			            {
			            	if(!itemList[n]->isSpecialFunctionItem())
			            	{
								bool is_selected = (n == currSelectedItemIndex);
				                if (ImGui::Selectable(itemList[n]->m_desc.c_str(), is_selected))
				                {
					                currSelectedItemIndex = n;
				                	// newItemName = itemList[n]->m_name.c_str();
				                	sprintf_s(newItemName, 128,"%s_New", itemList[n]->m_name.c_str());
				                	sprintf_s(newItemTitle, 128,"%s_New", itemList[n]->m_desc.c_str());
				                }
			
				                if (is_selected)
				                    ImGui::SetItemDefaultFocus();   // Set the initial focus when opening the combo (scrolling + for keyboard navigation support in the upcoming navigation branch)
			            	}
			            }
			            ImGui::EndCombo();
			        }
					
					ImGui::InputText("New Block Unqiue Name", newItemName, 128);
					ImGui::InputText("New Block Name Title", newItemTitle, 128);
					if(ImGui::Button("Make"))
					{
						//pass
						auto item = itemList[currSelectedItemIndex];
						GameItem * newItem = new GameItem(*item);
						newItem->m_name = newItemName;
						newItem->m_desc = newItemTitle;
						newItem->m_tintColor = GameWorld::shared()->getPlayer()->getPaintGun()->color;
						newItem->m_surfaceName = GameWorld::shared()->getPlayer()->getPaintGun()->m_surface->getName();
						auto part = new GamePart();
						part->initFromItem(newItem);

						newItem->m_thumbNail = new ThumbNail(part->getNode());
						GraphicsRenderer::shared()->updateThumbNail(newItem->m_thumbNail);
						GameItemMgr::shared()->pushItem(newItem);
					}
					ImGui::TreePop();
				}
				ImGui::EndTabItem();
			}
	        if (ImGui::BeginTabItem(TRC(u8"Terrain Surface"), 0, ImGuiTabItemFlags_None))
			{
				//表面材质
				ImGui::TextUnformatted(TRC(u8"地形材质选择"));
				auto size = PartSurfaceMgr::shared()->getItemAmount();
				for(int i = 0; i < 16; i++)
				{
					auto p = GameWorld::shared()->getPlayer()->getPaintGun();
					char tmp[128];
					sprintf_s(tmp,"mat :%d",i);
					if(ImGui::RadioButton(tmp, i ==p->m_matIndex))
					{
						p->m_matIndex = i;
					}
					if(i%2 == 0)
					{
						ImGui::SameLine();
					}
				}
				ImGui::EndTabItem();
			}
	        ImGui::EndTabBar();
		}
		ImGui::End();
	}
	PainterUI::PainterUI():m_onCreate(nullptr)
	{
	}

	void PainterUI::prepare()
	{
		
	}

}