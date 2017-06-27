#pragma once
#include "SimTile.h"
#include "Base/Node.h"
namespace tzw
{
	class SimMap
	{
	public:
		SimMap();
		void build();
		SimTile * getTile(int x, int y);
		Node * getRenderNode();
	private:
		SimTile * m_tiles[64][64];
		Node * m_root;
	};
}