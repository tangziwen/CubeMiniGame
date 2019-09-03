#pragma once
#include "2D/GUISystem.h"
#include "GameNodeEditorNode.h"
#include "rapidjson/document.h"
#include "Base/GuidObj.h"

namespace tzw {



class GameNodeEditor : public IMGUIObject
{
public:
	// Í¨¹ý IMGUIObject ¼Ì³Ð
	void drawIMGUI(bool * isOpen);

	void addNode(GameNodeEditorNode * newNode);
	void removeNode(GameNodeEditorNode * node);
	void removeAllLink(GameNodeEditorNode * node);
	void raiseEventToNode(int startAttr, int endAttr);
	void handleLinkDump(rapidjson::Value &partDocObj, rapidjson::Document::AllocatorType& allocator);
	void handleLinkLoad(rapidjson::Value &partDocObj);

	void makeLinkByNode(GameNodeEditorNode * NodeA, GameNodeEditorNode * NodeB, int indexOfA, int indeOfB);
protected:
	std::vector<GameNodeEditorNode * > m_gameNodes;
	std::vector<std::pair<int, int>> m_links;
};
}
