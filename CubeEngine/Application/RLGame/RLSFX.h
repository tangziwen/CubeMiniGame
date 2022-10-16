#pragma once
#include <list>
#include <vector>


#include "Base/Singleton.h"
#include "Math/vec2.h"
#include "2D/SpriteInstanceMgr.h"
namespace tzw
{

struct RLSFXSpec
{
	std::string filepath;
	float duration;
	float scale = 1.f;
};
struct RLSFX
{
	RLSFXSpec m_spec;
	float m_time = 0.f;
	vec2 m_pos;
	SpriteInstanceInfo * m_sprite;
};

class RLSFXMgr: public Singleton<RLSFXMgr>
{
public:
	RLSFXMgr();
	RLSFX * addSFX(vec2 pos, RLSFXSpec spec);
	void tick(float dt);
private:
	std::vector<RLSFX*> m_sfxList;
};

}