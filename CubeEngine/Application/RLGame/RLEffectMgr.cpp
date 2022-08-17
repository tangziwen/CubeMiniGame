#include "RLEffectMgr.h"
#include "rapidjson/document.h"
#include "Utility/file/Tfile.h"
#include "Utility/log/Log.h"
namespace tzw
{
	RLEffectInstance::RLEffectInstance(TObjectReflect * owner, RLEffect* effect)
		:m_data(effect),m_owner(owner),m_currentTime(0.f), m_currentPulseTime(0.f)
	{
	}

	void RLEffectInstance::apply()
	{
		
		for(const RLModifier & modifer : m_data->getModifierList())
		{
			float modifiedValue = modifer.m_modifiedValue;
			if(m_data->getEffectType() == RLEffectType::Pulse)
			{
				modifiedValue *= m_data->getPulseRate();
			}
			switch(modifer.m_modifierType)
			{
			case RLModifierType::Add:
				m_owner->addPropByName(modifer.m_attributeName, modifiedValue);
				break;
			case RLModifierType::Assign:
				m_owner->setPropByName(modifer.m_attributeName, modifiedValue);
				break;
			case RLModifierType::Multiply:
				float oldVal = m_owner->getPropByName<float>(modifer.m_attributeName);
				m_owner->setPropByName(modifer.m_attributeName, oldVal * modifiedValue);
				break;
			}
		
		}
	}

	void RLEffectInstance::tick(float dt)
	{
		m_currentTime += dt;
		if(m_data->getEffectType() == RLEffectType::Pulse)//pulse
		{
			m_currentPulseTime += dt;
		
			if(m_currentPulseTime > m_data->getPulseRate())
			{
				apply();
				m_currentPulseTime = 0.f;
			}
		}

	}
	void RLEffectInstance::onLeave()
	{
	}

	void RLEffectMgr::loadConfig()
	{
		std::string filePath = "RL/Perks.json";
		rapidjson::Document doc;
		auto data = Tfile::shared()->getData(filePath, true);
		doc.Parse<rapidjson::kParseTrailingCommasFlag | rapidjson::kParseCommentsFlag>(data.getString().c_str());
		if (doc.HasParseError())
		{
			tlog("[error] get json data err! %s %d offset %d",
				filePath.c_str(),
				doc.GetParseError(),
				doc.GetErrorOffset());
			exit(1);
		}
		for(int i = 0; i < doc.Size(); i++)
		{
			auto& node = doc[i];
			//RLHeroData data;
			RLEffect * effect = new RLEffect();

			std::string effectTypeStr = node["Type"].GetString();
			if(effectTypeStr == "Immediately")
			{
				effect->setEffectType(RLEffectType::Immediately);
			}
			else if(effectTypeStr == "Pulse")
			{
				effect->setEffectType(RLEffectType::Pulse);
			}
			effect->setDuration(node["Duration"].GetFloat());
			auto & modifierList = node["Modifiers"];
			effect->setSpritePath(node["SpritePath"].GetString());
			for(int j = 0; j < modifierList.Size(); j++)
			{
				auto& modifierNode = modifierList[j];
				RLModifier modifer;
				modifer.m_attributeName = modifierNode["Attribute"].GetString();
				std::string typeStr = modifierNode["Type"].GetString();
				if(typeStr == "Add")
				{
					modifer.m_modifierType = RLModifierType::Add;
				}
				modifer.m_modifiedValue = modifierNode["Value"].GetFloat();
				effect->addModifier(modifer);
				
			}
			m_effectPool[node["Name"].GetString()] = effect;
		}
	}
	
	RLEffectInstance* RLEffectMgr::getInstance(TObjectReflect * owner, std::string effectName)
	{
		RLEffectInstance * m_instance = new RLEffectInstance(owner, m_effectPool[effectName]);
		return m_instance;
	}

	RLEffectInstance* RLEffectMgr::getInstance(TObjectReflect* owner, RLEffect* effect)
	{
		RLEffectInstance * m_instance = new RLEffectInstance(owner, effect);
		return m_instance;
	}

	void RLEffectContainer::addEffectInstance(RLEffectInstance* instance)
	{
		instance->apply();
		if(instance->getEffect()->getEffectType() == RLEffectType::Immediately)
		{
			delete instance;
		}
		else
		{
			m_effectInstanceList.push_back(instance);
		}
	}
	void RLEffectContainer::tick(float dt)
	{
		for(auto iter = m_effectInstanceList.begin(); iter != m_effectInstanceList.end();)
		{
		
			RLEffectInstance * instance = *iter;
			instance->tick(dt);
			if(instance->isOutOfTime())
			{
				instance->onLeave();
				m_effectInstanceList.erase(iter);
			}
			else
			{
				iter ++;
			}
		
		}
	}
}