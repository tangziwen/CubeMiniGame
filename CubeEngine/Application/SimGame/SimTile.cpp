#include "SimTile.h"
#include "Engine/Engine.h"
#include "Scene/SceneMgr.h"
#include "SimMap.h"
namespace tzw
{
	SimTile::SimTile(int x, int y, SimMap * parent)
	{
		m_x = x;
		m_y = y;
		m_parent = parent;
		m_sprite = new Sprite();
		m_parent->getRenderNode()->addChild(m_sprite, false);
		m_sprite->setPos2D(x * 64, y * 64);
	}

	void SimTile::add()
	{

	}

	void SimTile::build()
	{
		m_sprite->initWithTexture(Engine::shared()->getUserPath("Sim/Textures/dirt.png"));
	}
}