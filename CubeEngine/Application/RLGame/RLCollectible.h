#pragma once
#include "Engine/EngineDef.h"
#include "Math/vec2.h"
#include <vector>
#include "2D/Collider2D.h"
namespace tzw
{
struct SpriteInstanceInfo;
class RLCollectible
{
public:
	RLCollectible(unsigned int typeID, vec2 Pos);
	~RLCollectible();
	vec2 getPos();
	void setPos(vec2 pos);
	void initGraphics();
	void onCollision(Collider2D * self, Collider2D * other);
	Collider2D * m_collider = nullptr;
	bool getIsAlive();
private:
	unsigned int m_typeID = 0;
	vec2 m_pos;
	SpriteInstanceInfo * m_sprite = nullptr;
	bool m_isAlive = true;
	
};

class RLCollectibleMgr : public Singleton<RLCollectibleMgr>
{
public:
	RLCollectibleMgr();
	void addCollectible(int typeID, vec2 pos);
	SpriteInstanceInfo * giveGraphics();
	void removeGraphics(SpriteInstanceInfo * info);
	void tick(float dt);
private:
	std::vector<RLCollectible*> m_collectible;
	int m_collectibleSpriteType;
};


}