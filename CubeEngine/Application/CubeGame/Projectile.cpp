#include "Projectile.h"
#include "3D/Primitive/CubePrimitive.h"
#include "3D/Primitive/SpherePrimitive.h"
#include "EngineSrc/Scene/SceneMgr.h"
#include "Action/MoveBy.h"

namespace tzw
{


	Projectile::Projectile(ProjectileType type)
	{
		m_type = type;
	}

	void Projectile::launch(vec3 from, vec3 direction, float speed)
	{
		
		//CubePrimitive * cube = new CubePrimitive(0.1, 0.1, 0.1);
		//g_GetCurrScene()->addNode(cube);
		//cube->setPos(from);
		//cube->runAction(new MoveBy(3.0, direction * speed));

		SpherePrimitive * sphere = new SpherePrimitive(3, 25);
		g_GetCurrScene()->addNode(sphere);
		sphere->setPos(from);
		sphere->runAction(new MoveBy(0.5, direction * speed));
	}
}