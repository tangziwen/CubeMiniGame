#include "PTMWorld.h"
#include "Scene/SceneMgr.h"
#include "EngineSrc/CubeEngine.h"
namespace tzw
{


	PTMWorld::PTMWorld()
	{
	}

	void PTMWorld::initMap()
	{
		m_mapRootNode = Node::create();
		g_GetCurrScene()->addNode(m_mapRootNode);
		m_mapCamera = new PTMMapCamera(m_mapRootNode);
		for(int i = 0; i < PTM_MAP_SIZE; i++)
		{
			for(int j = 0; j < PTM_MAP_SIZE; j++)
			{
				auto tile = new PTMTile();
				tile->coord_x = i;
				tile->coord_y = j;
				m_maptiles[i][j] = tile;
				tile->m_graphics->updateGraphics();
				if (rand() % 100 > 50)
				{
					auto town = new PTMTown(tile);
					m_pronviceList.push_back(town);
					town->updateGraphics();
				}
			}
		}
	}

	Node* PTMWorld::getMapRootNode()
	{
		return m_mapRootNode;
	}

}