#include "KeyTriggerNode.h"
#include "CubeGame/MainMenu.h"
#include "CubeGame/BehaviorNode.h"


namespace tzw
{
	KeyTriggerNode::KeyTriggerNode()
	{
		name =u8"按键输入";
		m_forwardAttr =addOutExe(u8"前后");
		m_sideAttr = addOutExe(u8"左右");
		m_forwardSignalAttr = addOut(u8"前后信号");
		m_sideSignalAttr = addOut(u8"左右信号");
		m_forward = 0;
		m_side = 0;
	}

	void KeyTriggerNode::trigger()
	{
		auto nodeEditor = MainMenu::shared()->getNodeEditor();
		std::vector<GameNodeEditorNode * > node_list;
		nodeEditor->findNodeLinksToAttr(m_forwardAttr, node_list);
		for(auto node : node_list)
		{
			if(node->getType() == Node_TYPE_BEHAVIOR)
			{
				static_cast<BehaviorNode *>(node)->execute();
			}
		}
	}

	void KeyTriggerNode::handleKeyPress(int keyCode)
	{
		bool isKeyActivate = false;
		switch(keyCode)
		{
		    case TZW_KEY_W: 
			{
				m_forward += 1;
				isKeyActivate = true;
		    }
			break;
		    case TZW_KEY_S: 
			{
				m_forward -= 1;
				isKeyActivate = true;
		    }
			break;
		    case TZW_KEY_A: 
			{
				m_side -= 1;
				isKeyActivate = true;
		    }
			break;
		    case TZW_KEY_D: 
			{
				m_side += 1;
				isKeyActivate = true;
		    }
			break;
		}
		m_forwardSignalAttr->m_localAttrValue.int_val = m_forward;
		m_sideSignalAttr->m_localAttrValue.int_val = m_side;
		trigger();
	}

	void KeyTriggerNode::handleKeyRelease(int keyCode)
	{
		bool isKeyActivate = false;
		switch(keyCode)
		{
		    case TZW_KEY_W: 
			{
				isKeyActivate = true;
				m_forward -= 1;
		    }
			break;
		    case TZW_KEY_S: 
			{
				isKeyActivate = true;
				m_forward += 1;
		    }
			break;
		    case TZW_KEY_A: 
			{
				m_side += 1;
				isKeyActivate = true;
		    }
			break;
		    case TZW_KEY_D: 
			{
				m_side -= 1;
				isKeyActivate = true;
		    }
			break;
		}
		m_forwardSignalAttr->m_localAttrValue.int_val = m_forward;
		m_sideSignalAttr->m_localAttrValue.int_val = m_side;
		trigger();
	}

	int KeyTriggerNode::getNodeClass()
	{
		return Node_CLASS_KEY_TRIGGER;
	}
}
