#include "KeyPairTriggerNode.h"
#include "CubeGame/GameUISystem.h"
#include "CubeGame/BehaviorNode.h"
#include "CubeGame/BuildingSystem.h"


namespace tzw
{
	KeyPairTriggerNode::KeyPairTriggerNode()
	{
		name =TR(u8"按键输入");
		m_onSignalChangedAttr = addOutExe(TR(u8"信号发生改变"));
		m_signalAttr = addOut(TR(u8"方向信号"));
		m_keyCodeA = TZW_KEY_N;
		m_keyCodeB = TZW_KEY_M;
		m_signal = 0;
	}

	void KeyPairTriggerNode::trigger()
	{

	}

	void KeyPairTriggerNode::privateDraw()
	{
		{
			int intValue = m_keyCodeA;
			ImGui::PushItemWidth(80);
			bool isInput = ImGui::InputInt("Key -1",&intValue);
			ImGui::PopItemWidth();
			if(isInput)
			{
				m_keyCodeA = intValue;
			}
		}
		{
			int intValue = m_keyCodeB;
			ImGui::PushItemWidth(80);
			bool isInput = ImGui::InputInt("Key 1",&intValue);
			ImGui::PopItemWidth();
			if(isInput)
			{
				m_keyCodeB = intValue;
			}
		}
	}

	void KeyPairTriggerNode::handleKeyPress(int keyCode)
	{
		if(m_keyCodeA == keyCode || m_keyCodeB == keyCode)
		{
			if(keyCode == m_keyCodeA)
			{
				m_signal -= 1;
				m_signal = std::clamp(m_signal, -1, 1);
			}
			if(keyCode == m_keyCodeB)
			{
				m_signal += 1;
				m_signal = std::clamp(m_signal, -1, 1);
			}
			m_signalAttr->m_localAttrValue.setInt(m_signal);
			triggerSignalChanged();
		}
	}

	void KeyPairTriggerNode::handleKeyRelease(int keyCode)
	{
		if(m_keyCodeA == keyCode || m_keyCodeB == keyCode)
		{
			if(keyCode == m_keyCodeA)
			{
				m_signal += 1;
				m_signal = std::clamp(m_signal, -1, 1);
			}
			if(keyCode == m_keyCodeB)
			{
				m_signal -= 1;
				m_signal = std::clamp(m_signal, -1, 1);
			}
			m_signalAttr->m_localAttrValue.setInt(m_signal);
			triggerSignalChanged();
		}
	}

	int KeyPairTriggerNode::getNodeClass()
	{
		return Node_CLASS_KEY_PAIR;
	}

	void KeyPairTriggerNode::load(rapidjson::Value& partData)
	{
		GameNodeEditorNode::load(partData);
		m_keyCodeA = partData["KeyCodeA"].GetInt();
		m_keyCodeB = partData["KeyCodeB"].GetInt();
	}

	void KeyPairTriggerNode::dump(rapidjson::Value& partDocObj, rapidjson::Document::AllocatorType& allocator)
	{
		GameNodeEditorNode::dump(partDocObj, allocator);
		partDocObj.AddMember("KeyCodeA", m_keyCodeA, allocator);
		partDocObj.AddMember("KeyCodeB", m_keyCodeB, allocator);
	}

	void KeyPairTriggerNode::triggerSignalChanged()
	{
		//if(!isPlayerOnSeat()) return;
		auto nodeEditor = GameUISystem::shared()->getNodeEditor();
		std::vector<GameNodeEditorNode * > node_list;
		nodeEditor->findNodeLinksToAttr(m_onSignalChangedAttr, node_list);
		for(auto node : node_list)
		{
			if(node->getType() == Node_TYPE_BEHAVIOR)
			{
				nodeEditor->pushToStack(node);
			}
		}
	}
	bool KeyPairTriggerNode::isPlayerOnSeat()
	{
		return BuildingSystem::shared()->getCurrentControlPart() && BuildingSystem::shared()->getCurrentControlPart()->getIsActivate();
	}
}
