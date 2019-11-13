#pragma once
#include "Engine/EngineDef.h"
#include "Base/Node.h"
#include "BlockPart.h"
#include <set>
#include "Island.h"
#include "Math/Ray.h"
#include "PartSurface.h"
#include <map>
namespace tzw
{

class PartSurfaceMgr
{
	TZW_SINGLETON_DECL(PartSurfaceMgr)
public:
	PartSurfaceMgr();
	void loadFromFile(std::string filePath);
	PartSurface * getItem(std::string name);
	int getItemAmount();
	PartSurface * getItemByIndex(int index);
	void addItem(std::string name, PartSurface * item);
private:
	std::map<std::string, PartSurface * > m_itemMap;
	std::vector<PartSurface * > m_itemList;
};


}
