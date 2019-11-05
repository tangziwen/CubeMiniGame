#pragma once
#include "2D/GUISystem.h"
#include <functional>

namespace tzw
{


	struct VehicleFileInfo_broswer
	{
		std::string fileName;
	};
	class VehicleBroswer : public IMGUIObject
	{
	public:
		// Í¨¹ý IMGUIObject ¼Ì³Ð
		void drawIMGUI(bool * isOpen) override;
		VehicleBroswer();
		void refreshDir(std::string dir);
		void open();
		std::function<void(std::string)> m_saveCallBack;
		std::function<void(std::string)> m_loadCallBack;
    private:
		bool m_loadOpen;
		bool m_saveOpen;
		std::vector<VehicleFileInfo_broswer *> m_fileNameList;
		std::string m_currSelected;
		std::string m_currDir;
	};

}
