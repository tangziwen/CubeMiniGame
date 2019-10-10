#pragma once
#include "2D/GUISystem.h"
#include "GameNodeEditorNode.h"
#include "rapidjson/document.h"
#include "Base/GuidObj.h"

namespace tzw {

struct LinkInfo
{
	int	Id;
	int  InputId;
    int  OutputId;
};

class GameNodeEditor : public IMGUIObject
{
public:
	GameNodeEditor();
	// Í¨¹ý IMGUIObject ¼Ì³Ð
	void drawIMGUI(bool * isOpen);

	void addNode(GameNodeEditorNode * newNode);
	void removeNode(GameNodeEditorNode * node);
	void removeAllLink(GameNodeEditorNode * node);
	void raiseEventToNode(int startAttr, int endAttr);
	void handleLinkDump(rapidjson::Value &partDocObj, rapidjson::Document::AllocatorType& allocator);
	void handleLinkLoad(rapidjson::Value &partDocObj);
	int getNodeIdByNode(GameNodeEditorNode * node);
	void makeLinkByNode(GameNodeEditorNode * NodeA, GameNodeEditorNode * NodeB, int indexOfA, int indeOfB);
	void newNodeEditorDraw(bool * isOpen);
	NodeAttr * findAttr(int attrID);
	void drawPinIcon(const NodeAttr* pin, bool connected, int alpha);
	void ShowLeftPane(float paneWidth);
	GameNodeEditorNode * findNode(int nodeID);
protected:
	std::vector<GameNodeEditorNode * > m_gameNodes;
	std::vector<LinkInfo> m_links;
};
}
