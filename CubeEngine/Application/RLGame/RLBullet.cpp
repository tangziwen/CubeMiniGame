#include "RLBullet.h"

namespace tzw
{
	RLBullet::RLBullet()
	{
		m_collider2D.m_cb = [this](Collider2D* self, Collider2D*other)
		{
			onCollision(self, other);
		};
	}
	void RLBullet::onCollision(Collider2D* self, Collider2D* other)
	{
		printf("bullet Hit!!!\n");
	}
}
