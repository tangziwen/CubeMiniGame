#include "Vehicle.h"

#include "Base/GuidMgr.h"
#include "GamePart.h"
#include "Island.h"
#include "GameConstraint.h"
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

Vehicle::~Vehicle()
{

	for(auto island : m_islandList)
	{
		for (auto& iter : island->m_partList)
		{
			if (iter->isConstraint())
			{
				auto bearing = static_cast<GameConstraint*>(iter);
				m_constrainList.erase(std::find(m_constrainList.begin(), m_constrainList.end(),bearing));
			}
			delete iter;
		}
		
		island->removeAll();
		m_islandList.erase(std::find(m_islandList.begin(), m_islandList.end(),island));
		delete island;
	}
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

void Vehicle::removeGamePart(GamePart* part)
{
	auto island = part->m_parent;
	if (part->isConstraint())
	{
		auto bearing = static_cast<GameConstraint*>(part);
		m_constrainList.erase(std::find( m_constrainList.begin(), m_constrainList.end(), bearing));
	}
	island->remove(part);
	delete part;
	//is the last island, remove it
	if(island->m_partList.empty())
	{
		m_islandList.erase(std::find(m_islandList.begin(), m_islandList.end(), island));
		delete island;
	}
}
void Vehicle::removeIsland(Island* island)
{
	for (auto& iter : island->m_partList)
	{
		if (iter->isConstraint())
		{
			auto bearing = static_cast<GameConstraint*>(iter);
			m_constrainList.erase(std::find(m_constrainList.begin(), m_constrainList.end(), bearing));
		}
		delete iter;
	}
	island->removeAll();
	m_islandList.erase(std::find(m_islandList.begin(), m_islandList.end(), island));
	delete island;
}
void Vehicle::update(float dt)
{
	for (auto constrain : m_constrainList)
	{
		constrain->updateTransform(dt);
	}
}
void Vehicle::enablePhysics()
{
		// each island, for normal island we create a rigid, for constraint island, we create a constraint
	for (auto island : m_islandList)
	{
		//we need record the building rotation!!!!!
		island->recordBuildingRotate();
		island->enablePhysics(true);
	}
	for(auto constraint :m_constrainList)
	{
		constraint->enablePhysics(true);
	}
}
}

