#pragma once
#include <string>
#include <vector>
#include "Engine/EngineDef.h"
#include <unordered_map>

namespace tzw
{

class RLHero;
enum class RLModifierType
{
	Add,
	Multiply,
};
struct RLModifier
{
	RLModifierType m_modifierType = RLModifierType::Add;
	std::string m_attributeName;
	float m_modifiedValue = 0.f;

};

enum class RLEffectGrantType
{
	OnEnter,
	OnDash,
	AfterDash,
	OnGotHit,
	OnHitEnemy,
	OnEnemyKilled,
	OnFired,
	OnWalk,
	OnStill,
	OnDeflect,
};

class RLEffect
{
public:
	float getPulseRate(){return m_pulseRate;};
	void setPulseRate(float newVal){m_pulseRate = newVal;}
	float getDuration() {return m_duration;}
	void setDuration(float newVal){m_duration = newVal;}
	const std::vector<RLModifier> & getModifierList() {return m_modifierList;}; 
	void addModifier(const RLModifier & modifer) {m_modifierList.push_back(modifer);}
	void setSpritePath(std::string spritePath){ m_spritePath = spritePath;}
	std::string getSpritePath() {return m_spritePath;}
	void addGrantList(RLEffectGrantType type, std::string grantEffectName) {m_grantList[type] = grantEffectName;}
	std::unordered_map<RLEffectGrantType, std::string> & getGrantList() {return m_grantList;}
protected:
	float m_pulseRate = 0.25f;
	float m_duration = 1.0f;
	std::vector<RLModifier> m_modifierList;
	std::unordered_map<RLEffectGrantType, std::string> m_grantList;
	std::string m_spritePath;
};
class RLEffectContainer;
class RLEffectInstance
{
public:
	RLEffectInstance(RLHero * owner, RLEffect * effect, RLEffectContainer * container);
	void apply();
	void tick(float dt);
	void onLeave();
	RLEffect * getEffect() {return m_data;}
	bool isOutOfTime() {return m_data->getDuration() > 0.0 ? m_currentTime >= m_data->getDuration() : false;};
	void setOwner(RLHero * owner){m_owner = owner;};
	void triggerGrant(RLEffectGrantType grant);
	std::string getModifiedName(const std::string & attributeName, RLModifierType type);
private:
	RLEffect * m_data = nullptr;
	float m_currentTime;
	RLHero * m_owner = nullptr;
	RLEffectContainer * m_parentContainer;
};


class RLEffectMgr : public Singleton<RLEffectMgr>
{
public:
	void loadConfig();
	RLEffectMgr() = default;
	RLEffectInstance * getInstance(RLHero * owner, std::string, RLEffectContainer * container);
	RLEffectInstance * getInstance(RLHero * owner, RLEffect * effect, RLEffectContainer * container);
	RLEffect * get(std::string effectName) {return m_effectPool[effectName];}
	
protected:
	std::unordered_map<std::string, RLEffect*> m_effectPool;

};

class RLEffectContainer
{
public:
	void addEffectInstance(RLEffectInstance * instance);
	void tick(float dt);
	void trigger(RLEffectGrantType effectType);
	float modifier(std::string modifierName);
	void addModify(std::string name, float value);
protected:
	void calModifier();
	std::vector<RLEffectInstance * > m_effectInstanceList;
	std::unordered_map<std::string, float> m_modifierList;
};
}