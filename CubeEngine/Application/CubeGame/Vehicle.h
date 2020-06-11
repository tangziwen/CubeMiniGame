#pragma once
#include <string>
#include <vector>

#include "Base/GuidObj.h"
#include "GameNodeEditor.h"
namespace tzw
{
	class Island;
	class Vehicle : public GuidObj
	{
	public:
		GameNodeEditor* getEditor() const;
		std::string getName() const;
		void setName(const std::string& name);
		Vehicle();
	private:
		std::string m_name;
		std::vector<Island *> m_islandList;
		GameNodeEditor * m_editor;
	};
}
