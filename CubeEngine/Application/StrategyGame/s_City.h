#ifndef S_CITY_H
#define S_CITY_H
#include <string>
#include "Math/ivec2.h"
namespace tzw
{
	class s_City
	{
	public:
		s_City();
		std::string getName() const;
		void setName(std::string val);
		int getLevel() const;
		void setLevel(int val);
		tzw::ivec2 getPos() const;
		void setPos(tzw::ivec2 val);
		int getPopulation() const;
		void setPopulation(int val);
	private:
		std::string m_name;
		int m_level;
		ivec2 m_pos;
		int m_population;
	};

}

#endif
