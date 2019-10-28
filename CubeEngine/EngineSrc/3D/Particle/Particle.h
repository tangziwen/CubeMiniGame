#pragma once
#include "Math/vec3.h"

namespace tzw
{
	struct Particle
	{
		float size;
		float m_span;
		float m_curAge;
		vec3 m_pos;
		Particle();
		void step(float dt);
		bool isDead();
	};
}
