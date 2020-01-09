#include "IfNode.h"
#include "CubeGame/ResNode.h"
#include "../BearPart.h"
#include "VarNode.h"
#include "CubeGame/GameUISystem.h"

namespace tzw
{
	IfNode::IfNode()
	{
		name =TR(u8"IF");
		m_leftValAttr =addOutExe(TR(u8"Then"));
		m_rightValAttr =addOutExe(TR(u8"Else"));
		m_cond =addIn(TR(u8"Condition"));
	}

	NodeAttrValue IfNode::execute()
	{
		auto nodeEditor = GameUISystem::shared()->getNodeEditor();
		std::vector<GameNodeEditorNode * > node_listtrue, node_listfalse;
		nodeEditor->findNodeLinksToAttr(m_leftValAttr, node_listtrue);
		nodeEditor->findNodeLinksToAttr(m_rightValAttr, node_listfalse);
		auto valueA = m_cond->eval();
		if(valueA.getInt() ==1)
		{
			for(auto node : node_listtrue)
			{
				if(node->getType() == Node_TYPE_BEHAVIOR)
				{
					nodeEditor->pushToStack(node);
					//static_cast<BehaviorNode *>(node)->execute();
				}
			}
		}
		else if(valueA.getInt() == 0)
		{
			for(auto node : node_listfalse)
			{
				if(node->getType() == Node_TYPE_BEHAVIOR)
				{
					nodeEditor->pushToStack(node);
				}
			}
		}
		return m_out->m_localAttrValue;
	}

	int IfNode::getNodeClass()
	{
		return Node_CLASS_IF;
	}
}
