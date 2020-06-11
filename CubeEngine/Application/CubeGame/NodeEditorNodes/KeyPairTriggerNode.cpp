#include "KeyPairTriggerNode.h"
#include "CubeGame/GameUISystem.h"
#include "CubeGame/BehaviorNode.h"
#include "CubeGame/BuildingSystem.h"
#include "CubeGame/UI/KeyMapper.h"


namespace tzw
{
	KeyPairTriggerNode::KeyPairTriggerNode():m_isNeedOnSeat(true)
	{
		name =TR(u8"键对输入触发器");
		m_onSignalChangedAttr = addOutExe(TR(u8"当信号改变"));
		m_signalAttr = addOut(TR(u8"信号"));
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
			ImGui::Text("Signal -1 Key: %c", char(m_keyCodeA));
			ImGui::SameLine();
			if(ImGui::Button("Change##KeyCodeA"))
			{
				KeyMapper::shared()->open([this](int keyCode)
				{
					if(keyCode != this->m_keyCodeB)
					{
						this->m_keyCodeA = keyCode;
					}
				});
			}
			ImGui::PopItemWidth();
		}
		{
			int intValue = m_keyCodeB;
			ImGui::PushItemWidth(80);
			ImGui::Text("Signal  1 Key: %c", char(m_keyCodeB));
			ImGui::SameLine();
			if(ImGui::Button("Change##KeyCodeB"))
			{
				KeyMapper::shared()->open([this](int keyCode)
				{
					if(keyCode != this->m_keyCodeA)
					{
						this->m_keyCodeB = keyCode;
					}
				});
			}
			ImGui::PopItemWidth();
		}
		if(ImGui::RadioButton("Is Need On Seat", m_isNeedOnSeat))
		{
			m_isNeedOnSeat = !m_isNeedOnSeat;
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
		GraphNode::load(partData);
		m_keyCodeA = partData["KeyCodeA"].GetInt();
		m_keyCodeB = partData["KeyCodeB"].GetInt();
		m_isNeedOnSeat = partData["IsNeedOnSeat"].GetBool();
	}

	void KeyPairTriggerNode::dump(rapidjson::Value& partDocObj, rapidjson::Document::AllocatorType& allocator)
	{
		GraphNode::dump(partDocObj, allocator);
		partDocObj.AddMember("KeyCodeA", m_keyCodeA, allocator);
		partDocObj.AddMember("KeyCodeB", m_keyCodeB, allocator);
		partDocObj.AddMember("IsNeedOnSeat", m_isNeedOnSeat, allocator);
	}

	void KeyPairTriggerNode::triggerSignalChanged()
	{
		if(m_isNeedOnSeat)
		{
			if(!isPlayerOnSeat()) return;
		}
		auto nodeEditor = getNodeEditor();
		std::vector<GraphNode * > node_list;
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
