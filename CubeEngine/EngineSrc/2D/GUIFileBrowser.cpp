#include "GUIFileBrowser.h"
#include "imgui.h"
#include "dirent.h"
#include "Utility/log/Log.h"
#include "Engine/Engine.h"

static void list_directory (const char* dirname, std::vector<tzw::fileInfo_broswer *> &fileNameList)
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
				auto a = new tzw::fileInfo_broswer();
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
	GUIFileBrowser::GUIFileBrowser(): m_isOpen(false),m_callBack(nullptr)
	{
		m_currSelected = "";
		refreshDir("./Res/Vehicles/");
	}

	void GUIFileBrowser::refreshDir(std::string dir)
	{
		m_fileNameList.clear();
		list_directory(dir.c_str(), m_fileNameList);
		m_currDir = dir;
	}

	void GUIFileBrowser::open(std::string title, std::string okBtn)
	{
		m_isOpen = true;
		m_titleStr = title;
		m_okBtnStr = okBtn;
	}

	void GUIFileBrowser::close()
	{
		m_isOpen = false;
	}

	static char inputBuff[128];
	void GUIFileBrowser::drawIMGUI()
	{
		if(!m_isOpen) return;
		auto ss = Engine::shared()->winSize();
		ImGui::SetNextWindowPos(ImVec2(ss.x / 2.0, ss.y / 2.0));
		ImGui::Begin(m_titleStr.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize);
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
		if(ImGui::Button(m_okBtnStr.c_str())) 
		{
			if(m_callBack)
			{
				m_callBack(m_currDir + m_currSelected);
			}
		}
		ImGui::SameLine();
		if(ImGui::Button(u8"È¡Ïû")) m_isOpen = false;
		ImGui::End();
	}
}
