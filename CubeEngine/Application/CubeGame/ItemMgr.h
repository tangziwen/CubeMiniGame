#pragma once
#include "Engine/EngineDef.h"
#include "Base/Node.h"
#include "BlockPart.h"
#include <set>
#include "Island.h"
#include "Math/Ray.h"
#include "GameItem.h"
#include <map>
namespace tzw
{
class ItemMgr;

class ItemMgr
{
	TZW_SINGLETON_DECL(ItemMgr)
public:
	ItemMgr();
	void loadFromFile(std::string filePath);
	GameItem * getItem(std::string name);
	int getItemAmount();
	GameItem * getItemByIndex(int index);
	void pushItem(GameItem* item);
private:
	std::map<std::string, GameItem * > m_itemMap;
	std::vector<GameItem * > m_itemList;
};


}
