#include "RLHero.h"

#include "RLHeroCollection.h"
#include "RLWorld.h"
#include "RLPlayerState.h"
#include "RLCollectible.h"
#include "RLSpritePool.h"
#include "RLUtility.h"
#include "RLSkills.h"
#include "box2d.h"
#include "RLDirector.h"
//#include "RLSFX.h"
namespace tzw
{
constexpr float characterRadius = 0.5f;
#pragma optimize("", off)
RLHero::RLHero(int idType)
	:m_id(idType)
{
	m_collider = new Collider2D(16, vec2(0, 0));
	m_wrapper = RLUserDataWrapper(this, RL_OBJECT_TYPE_MONSTER);
	m_wrapper.m_cb = [this](b2Body* self, b2Body* other, b2Contact* contact)
	{
		//onCollision(self, other, contact);
	};
	
	m_heroData = RLHeroCollection::shared()->getHeroData(idType);
	m_HP = m_heroData->m_maxHealth;
	m_MAXHP = m_heroData->m_maxHealth;
	m_Speed = m_heroData->m_speed;
	if(!m_heroData->m_defaultWeapon.empty())
	{
		equipWeapon(new RLWeapon(m_heroData->m_defaultWeapon));
	}
	
}

RLHero::~RLHero()
{
	//delete m_sprite;
	RLSpritePool::shared()->get()->removeSprite(m_sprite);
	RLSpritePool::shared()->get()->removeSprite(m_hpBar);
	RLWorld::shared()->getB2DWorld()->DestroyJoint(m_frictionJoint);
	RLWorld::shared()->getB2DWorld()->DestroyBody(m_body);
	delete m_collider;
	delete m_weapon;
}

void RLHero::setPosition(vec2 pos)
{
	RLUtility::shared()->clampToBorder(pos);
	m_position = pos;
	
	//m_collider->setPos(pos);
	if(m_body)
	{
		m_body->SetTransform(b2Vec2(pos.x / 32.f, pos.y / 32.f), 0.f);
	}
}
const vec2& RLHero::getPosition()
{
	if(m_body)
	{
		b2Vec2 p = m_body->GetPosition();
		vec2 posInPixel = vec2(p.x * 32.f, p.y * 32.f);
		m_position = posInPixel;
		return posInPixel;
	}
	
	return m_position;
}

void RLHero::updateGraphics()
{
	//vec2 pos = m_collider->getPos();
	if(!m_isInitedGraphics)
	{
		initGraphics();
	}
	if(m_sprite)
	{
		m_sprite->pos = getPosition();
		m_hpBar->pos = getPosition() + vec2(0, -16);
		m_hpBar->scale = vec2(m_HP / m_MAXHP, 0.125f);
	}
}

void RLHero::initGraphics()
{
	int spriteType = RLSpritePool::shared()->get()->getOrAddType(m_heroData->m_sprite);
	SpriteInstanceInfo * info = new SpriteInstanceInfo();
	info->type = spriteType;
	m_sprite = info;
	m_sprite->layer = 2;
	m_sprite->pos = getPosition();
	RLSpritePool::shared()->get()->addTile(m_sprite);


	int hpBarType = RLSpritePool::shared()->get()->getOrAddType("RL/Sprites/HpBar.png");
	SpriteInstanceInfo * hpInfo = new SpriteInstanceInfo();
	hpInfo->type = hpBarType;
	m_hpBar = hpInfo;
	m_hpBar->layer = 3;
	m_hpBar->pos = getPosition();
	m_hpBar->scale = vec2(1, 0.125f);
	RLSpritePool::shared()->get()->addTile(m_hpBar);


	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(m_position.x / 32.f, m_position.y / 32.f);
	m_body = RLWorld::shared()->getB2DWorld() ->CreateBody(&bodyDef);

	m_body->GetUserData().pointer = (uintptr_t)(&m_wrapper);
	b2CircleShape dynamicSphere;
	dynamicSphere.m_radius = characterRadius;

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &dynamicSphere;
	fixtureDef.density = 1.0f; 
	fixtureDef.friction = 1.0f;
	fixtureDef.restitution = 0.5;
	if(m_isPlayerControll)
	{
		fixtureDef.filter.categoryBits = RL_PLAYER;
		fixtureDef.filter.maskBits = RL_OBSTACLE | RL_ENEMY | RL_ENEMY_BULLET;
	}
	else
	{
		fixtureDef.filter.categoryBits = RL_ENEMY;
		fixtureDef.filter.maskBits = RL_OBSTACLE | RL_PLAYER | RL_PLAYER_BULLET | RL_ENEMY;
	}
		
	
	m_body->CreateFixture(&fixtureDef);



	b2FrictionJointDef jd;
	jd.localAnchorA.SetZero();
	jd.localAnchorB.SetZero();
	jd.bodyA = RLWorld::shared()->getGroundBody();
	jd.bodyB = m_body;
	jd.collideConnected = true;
	jd.maxForce = 7;
	jd.maxTorque = 0.1f * 1 * 0.5 * 10;

	m_frictionJoint = RLWorld::shared()->getB2DWorld()->CreateJoint(&jd);

	m_isInitedGraphics = true;
}

void RLHero::onTick(float dt)
{

	m_container.tick(dt);
	if(m_currSkill)
	{
		m_currSkill->onTick(dt);
		if(m_currSkill->isFinished())
		{
			m_currSkill->raiseFinished();
			delete m_currSkill;
			m_currSkill = nullptr;
		}
	}
	updateGraphics();
	if(m_controller)
	{
		m_controller->tick(dt);
	}
	if(m_weapon && !m_isDash && !m_isDeflect)
	{
		m_weapon->onTick(dt);
	}
	//m_collider->setPos(m_pos);
	if(!m_collider->getParent())
	{
		RLWorld::shared()->getQuadTree()->addCollider(m_collider);
	}
	if(m_isDash)
	{
		m_dashSpeed = 12.f;
		float decaySpeed = 3.f;
		float fullSpeedTime = 0.125f;
		float totalTime = 0.70f;
		float firstHalf = totalTime * 0.5f;
		//invicible
		if(m_dashTimer <= firstHalf)
		{
			m_sprite->overLayColor = vec4(0.5, 0.5, 1.0, 1.0f);
		}
		else
		{
			if(!m_isEnterDashSecondHalf)
			{
				leavePhase();
				m_isEnterDashSecondHalf = true;
			}
			m_sprite->overLayColor = vec4(0.5, 0.5, 1.0, 0.7f);
		}


		if(m_dashTimer > totalTime)
		{
			m_sprite->overLayColor = vec4(1.0, 1.0, 1.0, 0.0);
			m_isDash = false;
			m_isEnterDashSecondHalf = false;
		}
		float decayTime = std::max(m_dashTimer - fullSpeedTime, 0.f);
		float ratio = 1.0f - std::min( decayTime/ (totalTime - fullSpeedTime), 1.f);
		ratio = powf(ratio, 1.5f);
		m_dashVelocity = m_dashDir * (m_dashSpeed *ratio + decaySpeed * (1 - ratio));//vec2(1 * m_dashSpeed* dt, 0);
		m_body->SetLinearVelocity(b2Vec2(m_dashVelocity.x, m_dashVelocity.y));

		m_dashTimer += dt;


	}

	if(m_isDeflect)
	{
		if(m_deflectTimer < 0.2f)
		{
			m_sprite->overLayColor = vec4(255 / 255.f, 138.f / 255.f, 54.f / 255, 1.0);

			//µ¯·´
			vec2 minV = getPosition() - vec2(32, 32);
			vec2 maxV = getPosition() + vec2(32, 32);
			std::vector<Collider2D*> colliderList;
			RLWorld::shared()->getQuadTree()->getRange(AABB2D(minV, maxV), colliderList);
			for(Collider2D * collider : colliderList)
			{
				//filter enemyBullet
				if(collider->getSourceChannel() == CollisionChannel2D_Bullet
					&& collider->getResponseChannel() == CollisionChannel2D_Player)
				{
					auto bullet = static_cast<RLBullet *> (collider->getUserData().m_userData);
					vec2 diff = bullet->m_pos - getPosition();
					if(diff.length() < 64)
					{
						bullet->m_velocity = bullet->m_velocity * -1;
						bullet->m_t = 0.f;
						bullet->m_sprite->type = 1;
						collider->setResponseChannel(CollisionChannel2D_Entity); 
					}

				}
			
			
			}
		
		}
		else
		{
			m_sprite->overLayColor = vec4(219.f / 255.f, 52.f / 255.f, 0, 1.0);
		
		}

		m_deflectTimer += dt;
	}
	
	if(getIsPlayerControll())
	{
		m_hitImmuneTimer += dt;
		if(m_isHitImmune && m_hitImmuneTimer > 0.35f)
		{
			m_isHitImmune = false;
			m_hitImmuneTimer = 0.f;
		}
	}

	float hitEffectThreshold = 0.08f;
	if (getIsPlayerControll())
	{
		hitEffectThreshold = 0.35f;
	}
	m_hitEffectTimer += dt;
	if(m_triggerHitEffect && m_hitEffectTimer > hitEffectThreshold)
	{
		m_sprite->overLayColor = vec4(1.0, 1.0, 1.0, 0.0);
		m_triggerHitEffect = false;
		m_hitEffectTimer = 0.f;
	}

	m_isMoving = false;

	

	m_HP = std::clamp(m_HP, 0.f, m_MAXHP);
}

void RLHero::equipWeapon(RLWeapon* weapon)
{
	m_weapon = weapon;
	m_weapon->setOwner(this);
}

RLWeapon * RLHero::getWeapon()
{
	return m_weapon;
}

Collider2D* RLHero::getCollider2D()
{
	return m_collider;
}

void RLHero::onCollision(b2Body* self, b2Body* other, b2Contact* contact)
{
	if(!getIsPlayerControll())
	{
		if(other->GetUserData().pointer)
		{
			RLUserDataWrapper * data = reinterpret_cast<RLUserDataWrapper * >(other->GetUserData().pointer);
			switch(data->m_tag)
			{
			case RL_OBJECT_TYPE_MONSTER:
			{
				
				RLHero * hero = reinterpret_cast<RLHero *>(data->m_userData);
				if(hero->getIsPlayerControll())
				{
					hero->receiveDamage(5);
				}
			}
				break;
			default:
			{
			}
			break;
			}
		}
	}
}

void RLHero::receiveDamage(float damage)
{
	if(m_isHitImmune) return;
	m_triggerHitEffect = true;
	m_hitEffectTimer = 0.f;
	if(getIsPlayerControll())
	{
		m_isHitImmune = true;
		m_sprite->overLayColor = vec4(219.f / 255.f, 52.f / 255.f, 0, 1.0);
	}
	else
	{
		m_sprite->overLayColor = vec4(1.0, 1.0, 1.0, 1.0);
	}

	m_HP -= damage;
	if(m_HP <= 0.f)
	{
		if(getIsPlayerControll())
		{
			RLWorld::shared()->setCurrGameState(RL_GameState::AfterMath);
		}
		else
		{
			RLDirector::shared()->getCurrentWavePtr()->regKill(m_heroData->m_id);
			RLPlayerState::shared()->addScore(10);

			/*
			if(rand() % 10 > 3)
			{
				if(rand() % 10 > 2)
				{
					RLCollectibleMgr::shared()->addCollectible(0, getPosition());
				}
				else
				{
					RLCollectibleMgr::shared()->addCollectible(2, getPosition());
				}

			}
			*/
		}
	}

}

bool RLHero::isAlive()
{
	return m_HP > 0.f;
}

void RLHero::setController(RLController* controller)
{
	m_controller = controller;
}


void RLHero::onPossessed()
{
	if(getIsPlayerControll())
	{
		m_collider->setSourceChannel(CollisionChannel2D_Player);
		m_collider->setResponseChannel(CollisionChannel2D_Entity | CollisionChannel2D_Player);
		m_collider->setFriction(10);
		m_collider->setMaxSpeed(120);
	}
	else
	{
		m_collider->setSourceChannel(CollisionChannel2D_Entity);
		m_collider->setResponseChannel(CollisionChannel2D_Player);
	}
}

void RLHero::doDash()
{
	if(m_isDash) return;
	m_dashSpeed = 150.0f;
	m_isDash = true;
	m_dashTimer = 0.0f;
	m_collider->setIsCollisionEnable(false);
	m_sprite->overLayColor = vec4(0.5, 0.5, 1.0, 1.0);
	m_container.trigger(RLEffectGrantType::OnDash);
	enterPhase();
	if(m_Mana > 30.0f)
	{
		m_Mana = std::max(15.0f, m_Mana - 10.0f);
	}
	else
	{
		m_HP = std::max(1.f, m_HP - 10.0f);
	
	}

	if (m_isMoving)
	{
		m_dashDir = m_moveDir;
	}
	else
	{
		m_dashDir = m_weapon->getShootDir();
	}

}

void RLHero::doMove(vec2 dir, float delta)
{
	if(m_isDash) return;
	if(m_isDeflect) return;
	m_moveDir = dir;
	m_isMoving = true;
	float targetSpeed = m_Speed+ m_container.modifier("MovementSpeed_Add");
	if(m_weapon)
	{
		if(!getIsPlayerControll() && m_weapon->isFiring())
		{
			targetSpeed *= 0.5f;
		}
	}
	vec2 moveTarget = dir * 5.5;
	b2Vec2 vel = m_body->GetLinearVelocity();

	float velChangeX = moveTarget.x - vel.x;
	float velChangeY = moveTarget.y - vel.y;
	float impulseX = m_body->GetMass() * velChangeX; //disregard time factor
	float impulseY = m_body->GetMass() * velChangeY; //disregard time factor

	
	bool isXNeed = (moveTarget.x > 0 && moveTarget.x > vel.x) || (moveTarget.x < 0 && moveTarget.x < vel.x);
	bool isYNeed = (moveTarget.y > 0 && moveTarget.y > vel.y) || (moveTarget.y < 0 && moveTarget.y < vel.y);

	vec2 forceDir = vec2(dir.x * isXNeed, dir.y * isYNeed);
	if(isXNeed || isYNeed)
	{
		forceDir = forceDir.normalized();
		forceDir = forceDir  * 16.f;
		m_body->ApplyForceToCenter(b2Vec2(forceDir.x, forceDir.y), true);
	}

	//m_body->ApplyLinearImpulseToCenter(b2Vec2(velChangeX, velChangeY), true);
	//setPosition(getPosition() + dir * delta * targetSpeed);
}

void RLHero::applyEffect(std::string name)
{
	RLEffectInstance * instance =  RLEffectMgr::shared()->getInstance(this, name, &m_container);
	m_container.addEffectInstance(instance);
}

void RLHero::applyEffect(RLEffect* effect)
{
	RLEffectInstance * instance =  RLEffectMgr::shared()->getInstance(this, effect, &m_container);
	m_container.addEffectInstance(instance);
}
class QueryCb :public b2QueryCallback
{
public:
	virtual bool ReportFixture(b2Fixture* fixture)
	{
	
		m_resultBodyList.push_back(fixture->GetBody());
		return true;
	}
std::vector<b2Body *> m_resultBodyList;
};

bool RLHero::startDeflect()
{
	//m_isDeflect = true;
	//m_deflectTimer = 0.f;
	
	vec2 dir = m_weapon->getShootDir();

	b2Vec2 p = m_body->GetPosition();
	
	vec2 targetPosition = vec2(p.x, p.y) + dir * 4.f;
	
	float explosiveRadius = 1.5f;
	b2AABB aabb;
	aabb.lowerBound = b2Vec2(-explosiveRadius + targetPosition.x, -explosiveRadius + targetPosition.y);
	aabb.upperBound = b2Vec2(explosiveRadius + targetPosition.x, explosiveRadius + targetPosition.y);
	QueryCb queryCB;
	RLWorld::shared()->getB2DWorld()->QueryAABB(&queryCB, aabb);
	//RLSFXSpec spec = {"RL/AirJab.png", 0.3f, explosiveRadius / 0.5f};
	//RLSFXMgr::shared()->addSFX(vec2(targetPosition.x * 32.f, targetPosition.y * 32.f), spec);
	for(b2Body * body : queryCB.m_resultBodyList)
	{
		if(body->GetFixtureList()[0].GetFilterData().categoryBits == RL_ENEMY)
		{
			b2Vec2 pos = body->GetPosition();
			b2Vec2 diff = pos - b2Vec2(targetPosition.x, targetPosition.y);
			float length = diff.Normalize();
			if(length < (explosiveRadius + characterRadius))
			{
				diff *= 7.f;
				//RLSFXSpec spec = {"RL/Explosive.png", 0.1f};
				//RLSFXMgr::shared()->addSFX(vec2(pos.x * 32.f, pos.y * 32.f), spec);
				body->SetLinearVelocity(b2Vec2(0, 0));
				body->ApplyLinearImpulseToCenter(diff, true);


				RLUserDataWrapper * data = reinterpret_cast<RLUserDataWrapper * >(body->GetUserData().pointer);


				RLHero * hero = reinterpret_cast<RLHero *>(data->m_userData);
				hero->receiveDamage(10.f);
			}
		}
	}
	return true;
}

void RLHero::endDeflect()
{
	m_sprite->overLayColor = vec4(1, 1, 1, 0.0);
	m_isDeflect = false;
	m_deflectTimer = 0.f;
}

RLHeroData* RLHero::getHeroData()
{
	return m_heroData;
}

RLSkillBase* RLHero::playSkill()
{
	m_currSkill = new RLSkillCharge(this);
	m_currSkill->onEnter();
	return m_currSkill;
}

RLSkillBase* RLHero::playSkillToTarget(RLHero* hero)
{
	m_currSkill = new RLSkillCharge(this);
	m_currSkill->getBlackBoard().writeData("SkillTarget", hero);
	m_currSkill->onEnter();
	return m_currSkill;
}

void RLHero::enterPhase()
{
	b2Filter filter;
	for ( b2Fixture* f = m_body->GetFixtureList(); f; f = f->GetNext() ) {
		filter = f->GetFilterData();
		filter.maskBits = RL_OBSTACLE;
		f->SetFilterData(filter);
	}
}

void RLHero::leavePhase()
{
	b2Filter filter;
	for ( b2Fixture* f = m_body->GetFixtureList(); f; f = f->GetNext() ) {
		filter = f->GetFilterData();
		if(m_isPlayerControll) 
		{
			filter.maskBits = RL_OBSTACLE | RL_ENEMY | RL_ENEMY_BULLET;
		}
		else
		{
			filter.maskBits = RL_OBSTACLE | RL_PLAYER | RL_PLAYER_BULLET | RL_ENEMY;
		}
		f->SetFilterData(filter);
	}
}



}
