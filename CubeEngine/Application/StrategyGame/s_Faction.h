#ifndef S_FACTION_H
#define S_FACTION_H
#include <string>
namespace tzw
{
	class s_Faction
	{
	public:
	
		int getMilitaryLevel() const;
		void setMilitaryLevel(int val);
		std::string getName() const;
		void setName(std::string val);
		int getGold() const;
		void setGold(int val);
		int getPoliticalLevel() const;
		void setPoliticalLevel(int val);
		int getStability() const;
		void setStability(int val);
	private:
		std::string m_name;
		int m_gold;
		int m_militaryLevel;
		int m_politicalLevel;
		int m_stability;
	};

}

#endif // !S_FACTION_H
