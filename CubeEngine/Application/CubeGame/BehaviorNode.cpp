#include "BehaviorNode.h"
#include "MainMenu.h"


namespace tzw
{
	BehaviorNode::BehaviorNode()
	{
		addInExe(u8" ");
		m_out = addOutExe(u8" ");
	}

	NodeAttrValue BehaviorNode::execute()
	{
		return NodeAttrValue();
	}

	vec3 BehaviorNode::getNodeColor()
	{
		return vec3(0.3, 0.3, 1.0);
	}

	int BehaviorNode::getType()
	{
		return Node_TYPE_BEHAVIOR;
	}

	void BehaviorNode::handleExeOut()
	{
		auto nodeEditor = MainMenu::shared()->getNodeEditor();
		std::vector<GameNodeEditorNode * > node_list;
		nodeEditor->findNodeLinksToAttr(m_out, node_list);
		for(auto node : node_list)
		{
			if(node->getType() == Node_TYPE_BEHAVIOR)
			{
				//static_cast<BehaviorNode *>(node)->execute();
				nodeEditor->pushToStack(node);
			}
		}
	}
}
