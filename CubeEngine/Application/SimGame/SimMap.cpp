#include "SimMap.h"
#include "Scene/SceneMgr.h"
namespace tzw {

	SimMap::SimMap()
	{
		m_root = new Node();
		g_GetCurrScene()->addNode(m_root);
	}

	void SimMap::build()
	{
		for (int x = 0; x < 64; x++)
		{
			for (int y = 0; y < 64; y++)
			{
				 auto tile = new SimTile(x, y, this);
				 tile->build();
				 m_tiles[x][y] = tile;
			}
		}
	}

	SimTile * SimMap::getTile(int x, int y)
	{
		return m_tiles[x][y];
	}
	Node * SimMap::getRenderNode()
	{
		return m_root;
	}
}