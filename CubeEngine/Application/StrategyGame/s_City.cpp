#include "s_City.h"
namespace tzw
{
	s_City::s_City()
	{

	}

	std::string s_City::getName() const
	{
		return m_name;
	}

	void s_City::setName(std::string val)
	{
		m_name = val;
	}

	int s_City::getLevel() const
	{
		return m_level;
	}

	void s_City::setLevel(int val)
	{
		m_level = val;
	}

	ivec2 s_City::getPos() const
	{
		return m_pos;
	}

	void s_City::setPos(tzw::ivec2 val)
	{
		m_pos = val;
	}

	int s_City::getPopulation() const
	{
		return m_population;
	}

	void s_City::setPopulation(int val)
	{
		m_population = val;
	}


}
