#pragma once
#include "RLController.h"
namespace tzw
{
	class RLHero;
	class RLAIController : public RLController
	{
	public:
		RLAIController();
		virtual void tick(float dt) override;
	protected:

	};


}