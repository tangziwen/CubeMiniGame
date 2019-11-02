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
		void drawIMGUI() override;
		VehicleBroswer();
		void refreshDir(std::string dir);
		void open();
		void close();
		bool isOpen();
		std::function<void(std::string)> m_saveCallBack;
		std::function<void(std::string)> m_loadCallBack;
    private:
		std::vector<VehicleFileInfo_broswer *> m_fileNameList;
		std::string m_currSelected;
		bool m_isOpen;
		std::string m_currDir;
	};

}
