#include "RLSkills.h"
#include "Math/vec2.h"
#include "RLHero.h"
#include "RLUtility.h"
namespace tzw
{


	void RLSkillBase::addCallBack(std::function<void(RLSkillBase*)> callBack)
	{
		m_callBackList.push_back(callBack);
	}

	void RLSkillBase::raiseFinished()
	{
		for(auto & cb : m_callBackList)
		{
			cb(this);
		}
	}

	void RLSkillCharge::onEnter()
	{

		//store the direction in black board
		RLHero* targetHero =  m_blackBoard.getData<RLHero *>("SkillTarget", nullptr);
		vec2 chargeDiff =  targetHero->getPosition() - m_parent->getPosition();
		float chargeDistance = m_blackBoard.getData("ChargeDistance", 300.f);
		//if(chargeDiff.length() > chargeDistance)
		//no matter how far just charge a fixed distance
		{
			chargeDiff = chargeDiff.normalized();
			chargeDiff *= chargeDistance;

		}
		vec2 targetPos = m_parent->getPosition() + chargeDiff;
		RLUtility::shared()->clampToBorder(targetPos);
		m_blackBoard.writeData("ChargeTarget", targetPos);
	}

	void RLSkillCharge::onTick(float dt)
	{
		vec2 currPos = m_parent->getPosition();
		vec2 target = m_blackBoard.getData("ChargeTarget", vec2());
		vec2 diff =  target - m_parent->getPosition();


		if(diff.length() < 16.f)//close enough
		{
			m_isFinished = true;//setCurrentTranscationFinish();//mark current transcation finish
		}
		else
		{
				
			diff = diff.normalized();
			//m_parent->setPosition(currPos + diff * dt * 300 / 32.f);
		}
	}

	bool RLSkillCharge::isFinished()
	{
		return m_isFinished;
	}



}