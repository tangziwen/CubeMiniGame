#pragma once
#include "2D/GUISystem.h"
#include "GraphNode.h"
#include "rapidjson/document.h"
#include "Base/GuidObj.h"
#include "TriggerNode.h"
#include <queue>
#include "NodeEditor/Include/imgui_node_editor.h"
namespace tzw {

struct LinkInfo
{
	int	Id;
	int  InputId;
    int  OutputId;
};

class GameNodeEditor : public IMGUIObject, public EventListener
{
public:
	GameNodeEditor();
	// Í¨¹ý IMGUIObject ¼Ì³Ð
	void drawIMGUI(bool * isOpen);

	void addNode(GraphNode * newNode);
	void addNodeInCenter(GraphNode * newNode);
	void removeNode(GraphNode * node);
	void removeAllLink(GraphNode * node);
	void raiseEventToNode(int startAttr, int endAttr);
	void dump(rapidjson::Value &partDocObj, rapidjson::Document::AllocatorType& allocator);
	void load(rapidjson::Value &partDocObj);
	int getNodeIdByNode(GraphNode * node);
	void makeLinkByNode(GraphNode * NodeA, GraphNode * NodeB, int indexOfA, int indeOfB);
	void newNodeEditorDraw(bool * isOpen);
	NodeAttr * findAttr(int attrID);
	void drawPinIcon(const NodeAttr* pin, bool connected, int alpha);
	void ShowLeftPane(float paneWidth);
	GraphNode * findNode(int nodeID);
	bool onKeyPress(int keyCode) override;
	bool onKeyRelease(int keyCode) override;
	void findNodeLinksToAttr(NodeAttr * attr, std::vector<GraphNode*>&nodeList);
	GraphNode * findNodeByAttr(NodeAttr * attr);
	GraphNode * findNodeByAttrGid(unsigned int gid);
	GraphNode * findNodeLinksFromAttr(NodeAttr * attr);
	NodeAttr * findAttrLinksFromAttr(NodeAttr * attr);
	std::vector<NodeAttr *> findAllAttrLinksFromAttr(NodeAttr * attr);
	void pushToStack(GraphNode * node);
	void clearAll();
	void navigateToContent();
	void onPressButtonNode(GraphNode * buttonNode);
	void onReleaseButtonNode(GraphNode * buttonNode);
	void onReleaseSwitchNode(GraphNode * buttonNode);
	GraphNode * createNodeByClass(int classID);
protected:
	std::queue<GraphNode *> m_rt_exe_chain;
	std::vector<GraphNode * > m_gameNodes;
	std::vector<LinkInfo> m_links;
	std::vector<TriggerNode * > m_triggerList;
	GraphNode * m_pasteBinNode;
	ax::NodeEditor::EditorContext* m_context;
};
}
