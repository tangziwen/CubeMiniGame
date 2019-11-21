#include "SwitchNode.h"
#include "CubeGame/MainMenu.h"
#include "CubeGame/BehaviorNode.h"
#include "CubeGame/BuildingSystem.h"


namespace tzw
{
	SwitchNode::SwitchNode()
	{
		name =TR(u8"开关输入");
		m_pressedAttr =addOutExe(TR(u8"打开"));
		m_ReleasedAttr = addOutExe(TR(u8"关闭"));
		m_keyCode = TZW_KEY_N;
	}

	void SwitchNode::trigger()
	{

	}

	void SwitchNode::privateDraw()
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

	void SwitchNode::handleKeyPress(int keyCode)
	{
		if(!m_keyCode) return;
		if(m_keyCode == keyCode)
		{
			triggerPress();
		}
	}

	void SwitchNode::handleKeyRelease(int keyCode)
	{
		if(!m_keyCode) return;
		if(m_keyCode == keyCode)
		{
			triggerRelease();
		}
	}

	int SwitchNode::getNodeClass()
	{
		return Node_CLASS_SWITCH;
	}

	void SwitchNode::triggerPress()
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

	void SwitchNode::triggerRelease()
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
	bool SwitchNode::isPlayerOnSeat()
	{
		return BuildingSystem::shared()->getCurrentControlPart() && BuildingSystem::shared()->getCurrentControlPart()->getIsActivate();
	}
}
