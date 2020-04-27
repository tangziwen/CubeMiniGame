#pragma once
#include "Engine/EngineDef.h"
#include "Base/Node.h"
#include "BlockPart.h"
#include <set>
#include "Island.h"
#include "Math/Ray.h"
#include "GameItem.h"
#include <map>
#include "Bullet.h"
namespace tzw
{

enum class BulletType
{
	Projecttile,
	HitScanLaser,
	HitScanTracer,
	PulseLaser,
};
class BulletMgr : public Singleton<BulletMgr>
{
public:
	BulletMgr();
	void handleDraw(float dt);
	Bullet* fire(vec3 fromPos, vec3 direction, float speed, BulletType bulletType);
private:
	std::vector<Bullet *> m_bullets;
	bool m_isShowAssistInfo;
};


}
