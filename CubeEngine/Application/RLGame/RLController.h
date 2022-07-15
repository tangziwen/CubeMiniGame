#pragma once
namespace tzw
{
	class RLHero;
	class RLController
	{
	public:
		RLController();
		virtual void possess(RLHero * hero);
		virtual void tick(float dt);
	protected:
		RLHero * m_currPossessHero = nullptr;
	};


}