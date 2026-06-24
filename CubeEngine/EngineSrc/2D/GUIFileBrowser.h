#pragma once
#include "IMGUISystem.h"
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
		// 通过 IMGUIObject 继承
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
