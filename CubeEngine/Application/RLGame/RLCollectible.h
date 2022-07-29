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
	virtual void onCollision(Collider2D * self, Collider2D * other);
	Collider2D * m_collider = nullptr;
	bool getIsAlive();
	void setIsAlive(bool isAlive);
	virtual void onCollect();
protected:
	unsigned int m_typeID = 0;
	vec2 m_pos;
	SpriteInstanceInfo * m_sprite = nullptr;
	bool m_isAlive = true;
	
};
class RLCollectibleLevelUpPerk:public RLCollectible
{
public:
	RLCollectibleLevelUpPerk(unsigned int typeID, vec2 Pos);
	virtual void onCollect() override;

};
class RLCollectibleMgr : public Singleton<RLCollectibleMgr>
{
public:
	RLCollectibleMgr();
	RLCollectible * addCollectible(int typeID, vec2 pos);
	SpriteInstanceInfo * giveGraphics();
	void removeGraphics(SpriteInstanceInfo * info);
	void tick(float dt);
private:
	std::vector<RLCollectible*> m_collectible;
	int m_collectibleSpriteType;
};


}