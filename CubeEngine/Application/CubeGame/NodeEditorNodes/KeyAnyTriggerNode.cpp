#include "KeyAnyTriggerNode.h"
#include "CubeGame/GameUISystem.h"
#include "CubeGame/BehaviorNode.h"
#include "CubeGame/BuildingSystem.h"
#include "CubeGame/UI/KeyMapper.h"


namespace tzw
{
	KeyAnyTriggerNode::KeyAnyTriggerNode():m_isNeedOnSeat(true)
	{
		name =TR(u8"按键输入");
		m_pressedAttr =addOutExe(TR(u8"按下"));
		m_ReleasedAttr = addOutExe(TR(u8"松开"));
		m_keyCode = TZW_KEY_N;
	}

	void KeyAnyTriggerNode::trigger()
	{

	}

	void KeyAnyTriggerNode::privateDraw()
	{
		int intValue = m_keyCode;
		ImGui::PushItemWidth(80);
		ImGui::Text("Key %c", char(m_keyCode));
		ImGui::PopItemWidth();
		ImGui::SameLine();
		if(ImGui::Button("Change##KeyCode"))
		{
			KeyMapper::shared()->open([this](int keyCode){this->m_keyCode = keyCode;});
		}
		if(ImGui::RadioButton("Is Need On Seat", m_isNeedOnSeat))
		{
			m_isNeedOnSeat = !m_isNeedOnSeat;
		}
	}

	void KeyAnyTriggerNode::handleKeyPress(int keyCode)
	{
		if(!m_keyCode) return;
		if(m_keyCode == keyCode)
		{
			triggerPress();
		}
	}

	void KeyAnyTriggerNode::handleKeyRelease(int keyCode)
	{
		if(!m_keyCode) return;
		if(m_keyCode == keyCode)
		{
			triggerRelease();
		}
	}

	int KeyAnyTriggerNode::getNodeClass()
	{
		return Node_CLASS_KEY_ANY_TRIGGER;
	}

	void KeyAnyTriggerNode::load(rapidjson::Value& partData)
	{
		GraphNode::load(partData);
		m_keyCode = partData["KeyCode"].GetInt();
		m_isNeedOnSeat = partData["IsNeedOnSeat"].GetBool();
	}

	void KeyAnyTriggerNode::dump(rapidjson::Value& partDocObj, rapidjson::Document::AllocatorType& allocator)
	{
		GraphNode::dump(partDocObj, allocator);
		partDocObj.AddMember("KeyCode", m_keyCode, allocator);
		partDocObj.AddMember("IsNeedOnSeat", m_isNeedOnSeat, allocator);
	}

	void KeyAnyTriggerNode::triggerPress()
	{
		if(m_isNeedOnSeat)
		{
			if(!isPlayerOnSeat()) return;
		}
		auto nodeEditor = GameUISystem::shared()->getNodeEditor();
		std::vector<GraphNode * > node_list;
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

	void KeyAnyTriggerNode::triggerRelease()
	{
		if(m_isNeedOnSeat)
		{
			if(!isPlayerOnSeat()) return;
		}
		auto nodeEditor = GameUISystem::shared()->getNodeEditor();
		std::vector<GraphNode * > node_list;
		nodeEditor->findNodeLinksToAttr(m_ReleasedAttr, node_list);
		for(auto node : node_list)
		{
			if(node->getType() == Node_TYPE_BEHAVIOR)
			{
				nodeEditor->pushToStack(node);
			}
		}
	}
	bool KeyAnyTriggerNode::isPlayerOnSeat()
	{
		return BuildingSystem::shared()->getCurrentControlPart() && BuildingSystem::shared()->getCurrentControlPart()->getIsActivate();
	}
}
