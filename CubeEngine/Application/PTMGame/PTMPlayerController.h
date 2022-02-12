#pragma once
#include <unordered_set>
#include <vector>

#include "Engine/EngineDef.h"
#include "EngineSrc/Event/Event.h"
#include "PTMNationController.h"
namespace tzw
{
struct PTMTile;
class PTMTown;
class PTMNation;
class PTMArmy;
class PTMPlayerController : public PTMNationController
{
public:
	PTMPlayerController();

	void selectArmy(PTMArmy *m_army);
	void deSelect(PTMArmy * army);
	void deSelectAll();
	size_t getSelectedArmiesCount(){return m_selectedArmy.size();};
	void moveSelectArmiesTo(PTMTile * tile);
protected:
	std::unordered_set<PTMArmy *> m_selectedArmy;
};

}