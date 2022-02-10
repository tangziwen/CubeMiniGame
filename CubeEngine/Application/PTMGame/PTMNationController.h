#pragma once
#include "Engine/EngineDef.h"
#include "EngineSrc/Event/Event.h"

namespace tzw
{
	struct PTMTile;
	class PTMTown;
	class PTMNation;
	class PTMNationController
	{
	public:
		PTMNationController();
		void control(PTMNation * nation);
		PTMNation * getControlledNation() {return m_nation;};
	protected:
		PTMNation * m_nation;
	};

}