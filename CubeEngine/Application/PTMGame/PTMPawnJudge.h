#pragma once
#include "Base/Singleton.h"

namespace tzw
{
	class PTMPawn;
	class PTMPawnJudge: public Singleton<PTMPawnJudge>
	{
	public:
		PTMPawnJudge();
		void offensive(PTMPawn * attacker, PTMPawn * defender);
	protected:
		void offensive_siege_impl(PTMPawn * attacker, PTMPawn * defender);
	};

}
