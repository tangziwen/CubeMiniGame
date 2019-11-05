#include "VehicleBroswer.h"
// #include "imgui.h"

#include "Utility/log/Log.h"
#include "Engine/Engine.h"
#include "BuildingSystem.h"
#define NOMINMAX
#include "dirent.h"
static void list_directory (const char* dirname, std::vector<tzw::VehicleFileInfo_broswer *> &fileNameList)
{
	struct dirent** files;
	int i;
	int n;

	/* Scan files in directory */
	n = scandir(dirname, &files, NULL, alphasort);
	if (n >= 0)
	{
		/* Loop through file names */
		for (i = 0; i < n; i++)
		{
			struct dirent* ent;

			/* Get pointer to file entry */
			ent = files[i];

			
			/* Output file name */
			switch (ent->d_type)
			{
			case DT_REG:
				{
				auto a = new tzw::VehicleFileInfo_broswer();
				a->fileName = ent->d_name;
				tzw::tlog("%s\n", ent->d_name);
				fileNameList.push_back(a);
				}
				break;
			case DT_DIR:

				tzw::tlog("%s/\n", ent->d_name);
				break;

			case DT_LNK:

				tzw::tlog("%s@\n", ent->d_name);
				break;

			default:

				tzw::tlog("%s*\n", ent->d_name);
			}
		}

		/* Release file names */
		for (i = 0; i < n; i++)
		{
			free(files[i]);
		}
		free(files);
	}
	else
	{
		fprintf(stderr, "Cannot open %s (%s)\n", dirname, strerror(errno));
		exit(EXIT_FAILURE);
	}
}

namespace tzw
{
	VehicleBroswer::VehicleBroswer():m_saveCallBack(nullptr),m_loadCallBack(nullptr),m_loadOpen(true),
		m_saveOpen(false)
	{
		m_currSelected = "";
		refreshDir("./Res/Vehicles/");
	}

	void VehicleBroswer::refreshDir(std::string dir)
	{
		m_fileNameList.clear();
		list_directory(dir.c_str(), m_fileNameList);
		m_currDir = dir;
	}

	void VehicleBroswer::open()
	{
		std::vector<Island *> group;
		BuildingSystem::shared()->getIslandsByGroup(BuildingSystem::shared()->getLift()->m_effectedIslandGroup, group);
		if(group.size() > 0) //open save first, other wise, open load first
        {
			m_loadOpen = false;
			m_saveOpen = true;
        }else
        {
			m_loadOpen = true;
			m_saveOpen = false;
        }
	}
	
	static char inputBuff[128];
	void VehicleBroswer::drawIMGUI(bool * isOpen)
	{
		if(!(*isOpen)) return;
		auto ss = Engine::shared()->winSize();
		ImGui::SetNextWindowPos(ImVec2(ss.x / 2.0, ss.y / 2.0), ImGuiCond_Always, ImVec2(0.5, 0.5));
		ImGui::Begin(u8"载具浏览器", isOpen, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
		ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
        if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
        {
        	auto flag = ImGuiTabItemFlags_None;

            if (m_loadOpen = ImGui::BeginTabItem(u8"读取载具", 0, flag))
            {
				ImGui::BeginChild('ch', ImVec2(0, 200));
				int i = 0;
				for(auto a:m_fileNameList)
				{
					if(ImGui::Selectable(a->fileName.c_str(), m_currSelected == a->fileName.c_str())) 
					{
						m_currSelected = a->fileName.c_str();
						strcpy(inputBuff, a->fileName.c_str());
					}
					i++;
				}
				ImGui::EndChild();
				if(ImGui::InputText(":FileName", inputBuff, sizeof(inputBuff))) 
				{
					m_currSelected = inputBuff;
				}
				if(ImGui::Button(u8"读取")) 
				{
					if(m_loadCallBack)
					{
						m_loadCallBack(m_currDir + m_currSelected);
					}
					(*isOpen) = false;
				}
				ImGui::SameLine();
				if(ImGui::Button(u8"取消")) (*isOpen) = false;

            	ImGui::EndTabItem();
            }
        	flag = ImGuiTabItemFlags_None;
            if (m_saveOpen = ImGui::BeginTabItem(u8"保存载具", 0, flag))
            {
				ImGui::BeginChild('ch', ImVec2(0, 200));
				int i = 0;
				for(auto a:m_fileNameList)
				{
					if(ImGui::Selectable(a->fileName.c_str(), m_currSelected == a->fileName.c_str())) 
					{
						m_currSelected = a->fileName.c_str();
						strcpy(inputBuff, a->fileName.c_str());
					}
					i++;
				}
				ImGui::EndChild();
				if(ImGui::InputText(":FileName", inputBuff, sizeof(inputBuff))) 
				{
					m_currSelected = inputBuff;
				}
				if(ImGui::Button(u8"保存")) 
				{
					if(m_saveCallBack)
					{
						m_saveCallBack(m_currDir + m_currSelected);
					}
					(*isOpen) = false;
				}
				ImGui::SameLine();
				if(ImGui::Button(u8"取消")) (*isOpen) = false;

            	ImGui::EndTabItem();
            }
        	ImGui::EndTabBar();
        }
		ImGui::End();
	}
}
