#include "GameNodeEditorNode.h"
#include "2D/imnodes.h"

namespace tzw
{
	static int g_attr_uid = 0;

	static void increaseAttrGID()
	{
		g_attr_uid += 1;
	}
	NodeAttr* GameNodeEditorNode::addIn(std::string attrName)
	{
		increaseAttrGID();
		auto attr = new NodeAttr();
		attr->m_name = attrName;
		attr->gID = g_attr_uid;
		m_inAttr.push_back(attr);
		return attr;
	}

	NodeAttr* GameNodeEditorNode::addOut(std::string attrName)
	{
		increaseAttrGID();
		auto attr = new NodeAttr();
		attr->m_name = attrName;
		attr->gID = g_attr_uid;
		m_outAttr.push_back(attr);
		return attr;
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

	void GameNodeEditorNode::privateDraw()
	{

	}

	NodeAttr* GameNodeEditorNode::checkInNodeAttr(int gid)
	{
		for(auto attr: m_inAttr) 
		{
			if (attr->gID == gid) return attr;  
		}
		return nullptr;
	}

	NodeAttr* GameNodeEditorNode::checkOutNodeAttr(int gid)
	{
		for(auto attr: m_outAttr) 
		{
			if (attr->gID == gid) return attr;  
		}
		return nullptr;
	}

	void GameNodeEditorNode::onLinkOut(int startID, int endID, GameNodeEditorNode* other)
	{
	}

	void GameNodeEditorNode::onLinkIn(int startID, int endID, GameNodeEditorNode* other)
	{
	}

	NodeAttr* GameNodeEditorNode::getAttrByGid(int GID)
	{
		for(auto attr : m_inAttr) {
			if(attr->gID == GID)
				return attr;
		}

		for(auto attr : m_outAttr) {
			if(attr->gID == GID)
				return attr;
		}
		return nullptr;
	}

	GameNodeEditorNode::~GameNodeEditorNode()
	{

	}
}
