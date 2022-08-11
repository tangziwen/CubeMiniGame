#pragma once
#include <string>
#include <vector>
#include "Engine/EngineDef.h"
#include <unordered_map>

namespace tzw
{

enum class RLModifierType
{
	Assign,
	Add,
	Multiply,
};
struct RLModifier
{
	RLModifierType m_modifierType = RLModifierType::Add;
	std::string m_attributeName;
	float m_modifiedValue = 0.f;

};

enum class RLEffectType
{
	Immediately = 0,
	Duration, // be reversed after duration
	Pulse,
};

class RLEffect
{
public:
	float getPulseRate(){return m_pulseRate;};
	void setPulseRate(float newVal){m_pulseRate = newVal;}
	float getDuration() {return m_duration;}
	void setDuration(float newVal){m_duration = newVal;}
	void setEffectType(RLEffectType newEffectType) {m_effectType = newEffectType;}
	RLEffectType getEffectType(){return m_effectType;}
	const std::vector<RLModifier> & getModifierList() {return m_modifierList;}; 
	void addModifier(const RLModifier & modifer) {m_modifierList.push_back(modifer);}
protected:
	float m_pulseRate = 0.25f;
	float m_duration = 1.0f;
	std::vector<RLModifier> m_modifierList;
	RLEffectType m_effectType;
};

class RLEffectInstance
{
public:
	RLEffectInstance(TObjectReflect * owner, RLEffect * effect);
	void apply();
	void tick(float dt);
	void onLeave();
	RLEffect * getEffect() {return m_data;}
	bool isOutOfTime() {return m_data->getDuration() > 0.0 ? m_currentTime >= m_data->getDuration() : false;};
	void setOwner(TObjectReflect * owner){m_owner = owner;};
private:
	RLEffect * m_data = nullptr;
	float m_currentTime;
	float m_currentPulseTime;
	TObjectReflect * m_owner = nullptr;
};


class RLEffectMgr : public Singleton<RLEffectMgr>
{
public:
	void loadConfig();
	RLEffectMgr() = default;
	RLEffectInstance * getInstance(TObjectReflect * owner, std::string);
protected:
	std::unordered_map<std::string, RLEffect*> m_effectPool;

};

class RLEffectContainer
{
public:
	void addEffectInstance(RLEffectInstance * instance);
	void tick(float dt);
protected:
	std::vector<RLEffectInstance * > m_effectInstanceList;
};
}