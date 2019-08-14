#pragma once
#include "GUISystem.h"
#include <functional>

namespace tzw
{


	struct fileInfo_broswer
	{
		std::string fileName;
	};
	class GUIFileBrowser : public IMGUIObject
	{
	public:
		// Í¨¹ý IMGUIObject ¼Ì³Ð
		void drawIMGUI() override;
		GUIFileBrowser();
		void refreshDir(std::string dir);
		void open(std::string title, std::string okBtn);
		void close();
		std::function<void(std::string)> m_callBack;
    private:
		std::vector<fileInfo_broswer *> m_fileNameList;
		std::string m_currSelected;
		bool m_isOpen;
		std::string m_okBtnStr;
		std::string m_titleStr;
		std::string m_currDir;
	};

}
