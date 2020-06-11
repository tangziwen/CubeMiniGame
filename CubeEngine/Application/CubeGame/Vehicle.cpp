#include "Vehicle.h"

#include "Base/GuidMgr.h"

namespace tzw
{
GameNodeEditor* Vehicle::getEditor() const
{
	return m_editor;
}

std::string Vehicle::getName() const
{
	return m_name;
}

void Vehicle::setName(const std::string& name)
{
	m_name = name;
}

Vehicle::Vehicle():m_name(""),m_islandGroup("")
{
	m_editor = new GameNodeEditor();
}

void Vehicle::addIsland(Island* island)
{
	m_islandList.emplace_back(island);
}

void Vehicle::addConstraint(GameConstraint* constraint)
{
	m_constrainList.emplace_back(constraint);
}

std::string Vehicle::getIslandGroup() const
{
	return m_islandGroup;
}

void Vehicle::setIslandGroup(const std::string& islandGroup)
{
	m_islandGroup = islandGroup;
}

const std::vector<Island*>& Vehicle::getIslandList()
{
	return m_islandList;
}

const std::vector<GameConstraint*>& Vehicle::getConstraintList()
{
	return m_constrainList;
}

void Vehicle::genIslandGroup()
{
	m_islandGroup = GUIDMgr::shared()->genGUID();
}
}
