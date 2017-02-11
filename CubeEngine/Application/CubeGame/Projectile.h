#ifndef TZW_PROJECTILE_H
#define TZW_PROJECTILE_H

#include "Math/vec3.h"
namespace tzw
{
	class Projectile
	{
	public:
		enum class ProjectileType
		{
			SimpleBullet,
			SimpleArrow,
		};
		Projectile(ProjectileType type);
		void launch(vec3 from, vec3 direction, float speed);

	private:
		ProjectileType m_type;
	};
}





#endif