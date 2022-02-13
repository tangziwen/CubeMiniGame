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
			
			//m_sprite = Sprite::create("PTM/plain.png");
			PTMWorld::shared()->getMapRootNode()->addChild(m_sprite);
		}
		m_sprite->setPos2D(m_parent->getCanvasPos());
		if(m_parent->m_owner && m_parent->m_owner->getOwner())
		{
			float overLayFactor = 0.75f;
			vec3 color = m_parent->m_owner->getOwner()->getNationColor();
			/*
			int arrayoffset[3] = {-1, 0, 1};
			for(int i = 0; i < 3; i++)
			{
				for(int j = 0; j < 3; j++)
				{
					PTMTile * tile = PTMWorld::shared()->getTile(m_parent->coord_x + i, m_parent->coord_y + j);
					if(tile == m_parent) continue;//ignore Self
					if(tile == nullptr || tile->m_owner != m_parent->m_owner)
					{
						overLayFactor = 0.2f;
					}
				}
			}
			*/
			m_sprite->setOverLayColor(vec4(color.x, color.y, color.z, overLayFactor));
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
