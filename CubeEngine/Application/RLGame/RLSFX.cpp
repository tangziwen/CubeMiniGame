#include "RLSFX.h"
#include "RLSpritePool.h"
namespace tzw
{
	RLSFXMgr::RLSFXMgr()
	{
	}


	RLSFX* RLSFXMgr::addSFX(vec2 pos, RLSFXSpec spec)
	{
		RLSFX * sfx = new RLSFX();
		sfx->m_spec = spec;
		sfx->m_time = 0.f;
		int spriteType = RLSpritePool::shared()->get()->getOrAddType(spec.filepath);
		SpriteInstanceInfo * info = new SpriteInstanceInfo();
		info->type = spriteType;
		sfx->m_sprite = info;
		sfx->m_sprite->layer = 3;
		sfx->m_sprite->pos = pos;
		sfx->m_sprite->scale = vec2(spec.scale, spec.scale);
		sfx->m_sprite->alpha = 0.3f;
		sfx->m_pos = pos;
		RLSpritePool::shared()->get()->addTile(sfx->m_sprite);
		m_sfxList.push_back(sfx);
		return sfx;
	}
	void RLSFXMgr::tick(float dt)
	{
		for(auto iter = m_sfxList.begin(); iter != m_sfxList.end();)
		{
			RLSFX * sfx = (*iter);
			if(sfx->m_time > sfx->m_spec.duration)
			{
				RLSpritePool::shared()->get()->removeSprite(sfx->m_sprite);
				delete sfx;
				iter = m_sfxList.erase(iter);
			}
			else
			{
				sfx->m_time += dt;
				iter++;
			}
		
		
		}
	}
}