#pragma once
#include "2D/GUISystem.h"
#include "GameNodeEditorNode.h"

namespace tzw {



class GameNodeEditor : public IMGUIObject
{
public:
	// Í¨¹ý IMGUIObject ¼Ì³Ð
	void drawIMGUI(bool * isOpen);

	void addNode(GameNodeEditorNode * newNode);
 
protected:
	std::vector<GameNodeEditorNode * > m_gameNodes;
	std::vector<std::pair<int, int>> m_links;
};
}
