#include "Vehicle.h"

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

Vehicle::Vehicle():m_name("")
{
	m_editor = new GameNodeEditor();
}
}
