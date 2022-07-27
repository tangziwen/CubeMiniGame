#include "RLSpritePool.h"
namespace tzw
{
	RLSpritePool::RLSpritePool()
	{
	}

	void RLSpritePool::init(Node * node)
	{
		m_mgr = new SpriteInstanceMgr();
		m_mgr->setLocalPiority(2);
		node->addChild(m_mgr);
	}
	SpriteInstanceMgr* RLSpritePool::get()
	{
		return m_mgr;
	}
}