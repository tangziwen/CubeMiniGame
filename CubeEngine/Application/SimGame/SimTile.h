#pragma once
#include "2D/Sprite.h"
namespace tzw
{
	class SimMap;
	class SimTile
	{
	public:
		SimTile(int x, int y, SimMap * parent);
		void add();
		void build();
		int m_x, m_y;
	private:
		Sprite * m_sprite;
		SimMap * m_parent;

	};
}