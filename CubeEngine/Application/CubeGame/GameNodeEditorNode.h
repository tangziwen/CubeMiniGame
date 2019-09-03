#pragma once
#include "2D/GUISystem.h"
#include "Base/GuidObj.h"
#include "rapidjson/document.h"
namespace tzw {

struct NodeAttr
{
	std::string m_name;
	int gID;
	int tag;
};
struct GameNodeEditorNode: public GuidObj 
{
public:
	NodeAttr* addIn(std::string attrName);
	NodeAttr* addOut(std::string attrName);
	std::string name;
	GameNodeEditorNode();
	std::vector<NodeAttr *> & getInAttrs();
	std::vector<NodeAttr *> & getOuAttrs();
	virtual void privateDraw();
	NodeAttr * checkInNodeAttr(int gid);
	NodeAttr * checkOutNodeAttr(int gid);
	virtual void onLinkOut(int startID, int endID, GameNodeEditorNode * other);
	virtual void onLinkIn(int startID, int endID, GameNodeEditorNode * other);
	NodeAttr * getAttrByGid(int GID);
	virtual ~GameNodeEditorNode();
	int getInputAttrLocalIndexByGid(int GID);
	int getOutputAttrLocalIndexByGid(int GID);
	NodeAttr * getInByIndex(int localIndex);
	NodeAttr * getOutByIndex(int localIndex);
	virtual void load(rapidjson::Value& partData);
	virtual void dump(rapidjson::Value &partDocObj, rapidjson::Document::AllocatorType& allocator);
protected:
	std::vector<NodeAttr *> m_inAttr;
	std::vector<NodeAttr *> m_outAttr;
	std::map<int, int> m_inGlobalMap;
	std::map<int, int> m_OutGlobalMap;
};

}
