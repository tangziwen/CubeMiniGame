#include "PTMTech.h"

namespace tzw
{
PTMTech::PTMTech()
{
}

void PTMTech::loadFromFile(std::string filePath)
{
}

PTMTechState::PTMTechState(PTMNation* nation, PTMTech* tech)
	:m_tech(tech), m_parent(nation)
{
}

void PTMTechState::doProgress(int progressRate)
{
}
}
