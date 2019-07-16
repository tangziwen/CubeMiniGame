#pragma once
#include "2D/GUISystem.h"

namespace tzw {

struct GameNodeEditorNode
{
public:
	void addIn(std::string attrName);
	void addOut(std::string attrName);
	std::string name;
	GameNodeEditorNode();
private:
	std::map<int, int> m_inGlobalMap;
	std::map<int, int> m_OutGlobalMap;
};

}
