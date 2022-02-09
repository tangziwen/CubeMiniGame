#include "PTMTile.h"
#include "Scene/SceneMgr.h"
#include "EngineSrc/CubeEngine.h"
#include "EngineSrc/Event/EventMgr.h"
#include "PTMWorld.h"
namespace tzw
{


	PTMTileGraphic::PTMTileGraphic(PTMTile * parent):m_parent(parent),m_sprite(nullptr)
	{
	
	}

	void PTMTileGraphic::updateGraphics()
	{
		if(!m_sprite)
		{
			int rndNumber = rand() % 5;
			if(rndNumber == 0)
			{
				m_sprite = Sprite::create("PTM/forest.png");
			}
			if(rndNumber == 1)
			{
				m_sprite = Sprite::create("PTM/plain.png");
			}
			if(rndNumber == 2)
			{
				m_sprite = Sprite::create("PTM/water.png");
			}
			if(rndNumber == 3)
			{
				m_sprite = Sprite::create("PTM/plain.png");
			}
			if(rndNumber == 4)
			{
				m_sprite = Sprite::create("PTM/mountains.png");
			}
			
		}
		m_sprite->setPos2D(m_parent->coord_x * 32,  m_parent->coord_y * 32 );
		PTMWorld::shared()->getMapRootNode()->addChild(m_sprite);
	}

	PTMTile::PTMTile()
	{
		m_graphics = new PTMTileGraphic(this);
	}

}