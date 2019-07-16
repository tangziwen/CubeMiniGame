#include "GameNodeEditorNode.h"
#include "2D/imnodes.h"

namespace tzw
{
	static int g_attr_uid = 0;

	static void increaseAttrGID()
	{
		g_attr_uid += 1;
	}
	void GameNodeEditorNode::addIn(std::string attrName)
	{
		increaseAttrGID();
		auto attr = new NodeAttr();
		attr->m_name = attrName;
		attr->gID = g_attr_uid;
		m_inAttr.push_back(attr);
	}

	void GameNodeEditorNode::addOut(std::string attrName)
	{
		increaseAttrGID();
		auto attr = new NodeAttr();
		attr->m_name = attrName;
		attr->gID = g_attr_uid;
		m_outAttr.push_back(attr);
	}

	GameNodeEditorNode::GameNodeEditorNode()
	{
	}

	std::vector<NodeAttr*>& GameNodeEditorNode::getInAttrs()
	{
		return m_inAttr;
	}

	std::vector<NodeAttr*>& GameNodeEditorNode::getOuAttrs()
	{
		return m_outAttr;
	}
}
