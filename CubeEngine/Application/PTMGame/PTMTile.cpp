#include "PTMTile.h"
#include "Scene/SceneMgr.h"
#include "EngineSrc/CubeEngine.h"
#include "EngineSrc/Event/EventMgr.h"
#include "PTMWorld.h"
#include "PTMPawn.h"
namespace tzw
{


	PTMTileGraphic::PTMTileGraphic(PTMTile * parent):m_parent(parent),m_sprite(nullptr)
	{
	
	}

	void PTMTileGraphic::updateGraphics()
	{
		TileMap2DMgr * tileMgr = PTMWorld::shared()->getTileMgr();
		if(!m_sprite)
		{
			
			if(m_parent->m_tileType == PTMTileType::TILE_OCEAN)
			{
				
				m_sprite = tileMgr->addTile("PTM/water.png", m_parent->coord_x, m_parent->coord_y);
			}
			else
			{
				int rndNumber = rand() % 4;
				if(rndNumber == 0)
				{
					m_sprite = tileMgr->addTile("PTM/forest.png", m_parent->coord_x, m_parent->coord_y);//m_sprite = Sprite::create("PTM/forest.png");
				}
				if(rndNumber == 1)
				{
					m_sprite = tileMgr->addTile("PTM/plain.png", m_parent->coord_x, m_parent->coord_y);//m_sprite = Sprite::create("PTM/plain.png");
				}
				if(rndNumber == 2)
				{
					m_sprite = tileMgr->addTile("PTM/plain.png", m_parent->coord_x, m_parent->coord_y);//m_sprite = Sprite::create("PTM/plain.png");
				}
				if(rndNumber == 3)
				{
					m_sprite = tileMgr->addTile("PTM/mountains.png", m_parent->coord_x, m_parent->coord_y);//m_sprite = Sprite::create("PTM/mountains.png");
				}
			}

			
			//m_sprite = Sprite::create("PTM/plain.png");
			//PTMWorld::shared()->getMapRootNode()->addChild(m_sprite);
		}
		//m_sprite->setPos2D(m_parent->getCanvasPos());
		if(m_parent->m_owner && m_parent->m_owner->getOwner())
		{
			float overLayFactor = 0.85f;
			vec3 color = m_parent->m_owner->getOwner()->getNationColor();
			//m_sprite->setOverLayColor(vec4(color.x, color.y, color.z, overLayFactor));
			m_sprite->overLayColor = vec4(color.x, color.y, color.z, overLayFactor);
		}

		
	}

	PTMTile::PTMTile()
		:coord_x(0), coord_y(0)
	{
		m_graphics = new PTMTileGraphic(this);
	}

	vec2 PTMTile::getCanvasPos()
	{
		return vec2(coord_x * 32,  coord_y * 32 );
	}

	void PTMTile::setPawn(PTMPawn* pawn)
	{
		m_pawn = pawn;
		//pawn->setTile(this);
	}

	void PTMTile::removePawn()
	{
		m_pawn = nullptr;
	}
}
