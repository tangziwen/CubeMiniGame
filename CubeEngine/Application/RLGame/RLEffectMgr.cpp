#include "RLEffectMgr.h"
#include "RLHero.h"
#include "rapidjson/document.h"
#include "Utility/file/Tfile.h"
#include "Utility/log/Log.h"
namespace tzw
{
	RLEffectInstance::RLEffectInstance(RLHero * owner, RLEffect* effect, RLEffectContainer * container)
		:m_data(effect),m_owner(owner),m_currentTime(0.f), m_parentContainer(container)
	{
	}

	void RLEffectInstance::apply()
	{
		
		for(const RLModifier & modifer : m_data->getModifierList())
		{
			float modifiedValue = modifer.m_modifiedValue;
			switch(modifer.m_modifierType)
			{
			case RLModifierType::Add:
			{

				m_parentContainer->addModify(getModifiedName( modifer.m_attributeName, RLModifierType::Add), modifiedValue);
			}
				break;

			case RLModifierType::Multiply:
			{
				m_parentContainer->addModify(getModifiedName( modifer.m_attributeName, RLModifierType::Multiply), modifiedValue);
			}
				break;
			}
		
		}

	}

	void RLEffectInstance::tick(float dt)
	{
		m_currentTime += dt;
	}
	void RLEffectInstance::onLeave()
	{
	}
	void RLEffectInstance::triggerGrant(RLEffectGrantType grant)
	{
		auto & grantList = m_data->getGrantList();
		auto iter = m_data->getGrantList().find(grant);
		if(iter != grantList.end())
		{
			m_owner->applyEffect(iter->second);
		}
	}
	std::string RLEffectInstance::getModifiedName(const std::string &attributeName, RLModifierType type)
	{
		char tmp[128];
		switch(type)
		{
		case RLModifierType::Add:
			sprintf(tmp, "%s_Add",attributeName.c_str());
			return tmp;
			break;
		case RLModifierType::Multiply:
			sprintf(tmp, "%s_Mul",attributeName.c_str());
			return tmp;
			break;
		}
		return std::string();
	}
#define SET_GRANT_TYPE(THETYPE) else if(grantListNode.HasMember( #THETYPE))\
	{effect->addGrantList(RLEffectGrantType::THETYPE, grantListNode[#THETYPE].GetString());}
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
				else if(typeStr == "Multiply")
				{
					modifer.m_modifierType = RLModifierType::Multiply;
				}
				modifer.m_modifiedValue = modifierNode["Value"].GetFloat();
				effect->addModifier(modifer);

			}
			if(node.HasMember("Grant"))
			{
				auto& grantListNode = node["Grant"];

				if(grantListNode.HasMember("OnEnter"))
				{
					effect->addGrantList(RLEffectGrantType::OnEnter, grantListNode["OnEnter"].GetString());
				}
				SET_GRANT_TYPE(OnDash)
				SET_GRANT_TYPE(AfterDash)
				SET_GRANT_TYPE(OnGotHit)
				SET_GRANT_TYPE(OnHitEnemy)
				SET_GRANT_TYPE(OnEnemyKilled)
				SET_GRANT_TYPE(OnFired)
				SET_GRANT_TYPE(OnWalk)
				SET_GRANT_TYPE(OnStill)
				SET_GRANT_TYPE(OnDeflect)
			
			}
			m_effectPool[node["Name"].GetString()] = effect;
		}
	}
	
	RLEffectInstance* RLEffectMgr::getInstance(RLHero * owner, std::string effectName, RLEffectContainer * container)
	{
		RLEffectInstance * m_instance = new RLEffectInstance(owner, m_effectPool[effectName], container);
		return m_instance;
	}

	RLEffectInstance* RLEffectMgr::getInstance(RLHero* owner, RLEffect* effect, RLEffectContainer * container)
	{
		RLEffectInstance * m_instance = new RLEffectInstance(owner, effect, container);
		return m_instance;
	}


	void RLEffectContainer::addEffectInstance(RLEffectInstance* instance)
	{
		m_effectInstanceList.push_back(instance);
	}

	void RLEffectContainer::tick(float dt)
	{
		m_modifierList.clear();
		for(auto iter = m_effectInstanceList.begin(); iter != m_effectInstanceList.end();)
		{
			RLEffectInstance * instance = *iter;
			instance->apply();
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
	void RLEffectContainer::trigger(RLEffectGrantType grantType)
	{
		std::vector<RLEffectInstance * > readyToTrigger;
		for(auto iter = m_effectInstanceList.begin(); iter != m_effectInstanceList.end(); iter++)
		{
		
			RLEffectInstance * instance = *iter;
			auto & grantList = instance->getEffect()->getGrantList();
			if(grantList.find(grantType) != grantList.end())
			{
				//instance->apply();
				readyToTrigger.push_back(instance);
			}
		}

		for(RLEffectInstance * instance : readyToTrigger)
		{
		
			instance->triggerGrant(grantType);
		}
	}
	float RLEffectContainer::modifier(std::string modifierName)
	{
		auto iter = m_modifierList.find(modifierName);
		if(iter == m_modifierList.end()) return 0.f;
		return iter->second;
	}
	void RLEffectContainer::addModify(std::string name, float value)
	{
		auto iter = m_modifierList.find(name);
		if(iter == m_modifierList.end())
		{
			m_modifierList[name] = value;
		
		}
		else
		{
			m_modifierList[name] += value;
		
		}
	}
	void RLEffectContainer::calModifier()
	{
	}
}