#pragma once
#include "2D/GUISystem.h"

namespace tzw {

struct NodeAttr
{
	std::string m_name;
	int gID;
};
struct GameNodeEditorNode
{
public:
	void addIn(std::string attrName);
	void addOut(std::string attrName);
	std::string name;
	GameNodeEditorNode();
	std::vector<NodeAttr *> & getInAttrs();
	std::vector<NodeAttr *> & getOuAttrs();
private:
	std::vector<NodeAttr *> m_inAttr;
	std::vector<NodeAttr *> m_outAttr;
	std::map<int, int> m_inGlobalMap;
	std::map<int, int> m_OutGlobalMap;
};

}
