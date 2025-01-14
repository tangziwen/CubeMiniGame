#pragma once
#include "Engine/EngineDef.h"
#include "Math/vec2.h"
#include <vector>
#include "2D/Collider2D.h"
namespace tzw
{
struct SpriteInstanceInfo;
class RLHero;
class RLEffect;
class RLCollectible
{
public:
	RLCollectible(unsigned int typeID, vec2 Pos);
	virtual ~RLCollectible();
	vec2 getPos();
	void setPos(vec2 pos);
	virtual void onCollision(Collider2D * self, Collider2D * other);
	Collider2D * m_collider = nullptr;
	bool getIsAlive();
	void setIsAlive(bool isAlive);
	virtual void onCollect(RLHero * hero);
	virtual int getSpriteType();
	void setSprite(SpriteInstanceInfo * sprite) {m_sprite = sprite;};
	SpriteInstanceInfo * getSprite() {return m_sprite;}
protected:
	unsigned int m_typeID = 0;
	vec2 m_pos;
	SpriteInstanceInfo * m_sprite = nullptr;
	bool m_isAlive = true;
	
};


class RLCollectibleEXP:public RLCollectible
{
public:
	RLCollectibleEXP(unsigned int typeID, vec2 Pos);
	virtual void onCollect(RLHero * hero) override;
};

class RLCollectibleGold:public RLCollectible
{
public:
	RLCollectibleGold(unsigned int typeID, vec2 Pos);
	virtual void onCollect(RLHero * hero) override;
	int getSpriteType() override;
};

class RLCollectibleLevelUpPerk:public RLCollectible
{
public:
	RLCollectibleLevelUpPerk(unsigned int typeID, vec2 Pos);
	virtual void onCollect(RLHero * hero) override;
	int getSpriteType() override;
	void setEffect(RLEffect * effect);
private:
	RLEffect * m_effect = nullptr;

};
class RLCollectibleMgr : public Singleton<RLCollectibleMgr>
{
public:
	RLCollectibleMgr();
	RLCollectible * addCollectible(int typeID, vec2 pos);
	RLCollectible * addCollectiblePerkEffect(RLEffect * effect, vec2 pos);
	SpriteInstanceInfo * giveGraphics(int collectibleType);
	void removeGraphics(SpriteInstanceInfo * info);
	void tick(float dt);
private:
	std::vector<RLCollectible*> m_collectible;
	int m_collectibleSpriteType;
};


}