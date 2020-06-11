#pragma once
#include <string>
#include <vector>


#include "Base/GuidObj.h"
#include "GameNodeEditor.h"
namespace tzw
{
class GameConstraint;
class Island;
	class Vehicle : public GuidObj
	{
	public:
		GameNodeEditor* getEditor() const;
		std::string getName() const;
		void setName(const std::string& name);
		Vehicle();
		void addIsland(Island * island);
		void addConstraint(GameConstraint * constraint);
		std::string getIslandGroup() const;
		void setIslandGroup(const std::string& islandGroup);
		const std::vector<Island *>& getIslandList();
		const std::vector<GameConstraint *>& getConstraintList();
		void genIslandGroup();
	private:
		std::string m_name;
		std::vector<Island *> m_islandList;
		GameNodeEditor * m_editor;
		std::string m_islandGroup;
		std::vector<GameConstraint* > m_constrainList;
	};
}
