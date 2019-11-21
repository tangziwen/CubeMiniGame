#include "ButtonNode.h"
#include "CubeGame/MainMenu.h"
#include "CubeGame/BehaviorNode.h"
#include "CubeGame/BuildingSystem.h"


namespace tzw
{
	ButtonNode::ButtonNode()
	{
		name =TR(u8"按钮输入");
		m_pressedAttr =addOutExe(TR(u8"按下"));
		m_ReleasedAttr = addOutExe(TR(u8"松开"));
		m_buttonAttr = addIn(TR(u8"按钮部件"));
		m_keyCode = TZW_KEY_N;
	}

	void ButtonNode::trigger()
	{

	}

	void ButtonNode::privateDraw()
	{
		int intValue = m_keyCode;
		ImGui::PushItemWidth(80);
		bool isInput = ImGui::InputInt("",&intValue);
		ImGui::PopItemWidth();
		if(isInput)
		{
			m_keyCode = intValue;
			//m_attr->m_localAttrValue.setInt(intValue);
		}
	}

	void ButtonNode::handleKeyPress(int keyCode)
	{
		if(!m_keyCode) return;
		if(m_keyCode == keyCode)
		{
			triggerPress();
		}
	}

	void ButtonNode::handleKeyRelease(int keyCode)
	{
		if(!m_keyCode) return;
		if(m_keyCode == keyCode)
		{
			triggerRelease();
		}
	}

	int ButtonNode::getNodeClass()
	{
		return Node_CLASS_Button;
	}

	void ButtonNode::triggerPress()
	{
		//if(!isPlayerOnSeat()) return;
		auto nodeEditor = MainMenu::shared()->getNodeEditor();
		std::vector<GameNodeEditorNode * > node_list;
		nodeEditor->findNodeLinksToAttr(m_pressedAttr, node_list);
		for(auto node : node_list)
		{
			if(node->getType() == Node_TYPE_BEHAVIOR)
			{
				nodeEditor->pushToStack(node);
				//static_cast<BehaviorNode *>(node)->execute();
			}
		}
	}

	void ButtonNode::triggerRelease()
	{
		//if(!isPlayerOnSeat()) return;
		auto nodeEditor = MainMenu::shared()->getNodeEditor();
		std::vector<GameNodeEditorNode * > node_list;
		nodeEditor->findNodeLinksToAttr(m_ReleasedAttr, node_list);
		for(auto node : node_list)
		{
			if(node->getType() == Node_TYPE_BEHAVIOR)
			{
				nodeEditor->pushToStack(node);
				//static_cast<BehaviorNode *>(node)->execute();
			}
		}
	}
	bool ButtonNode::isPlayerOnSeat()
	{
		return BuildingSystem::shared()->getCurrentControlPart() && BuildingSystem::shared()->getCurrentControlPart()->getIsActivate();
	}
}
