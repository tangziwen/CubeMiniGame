#include "Particle.h"
namespace tzw
{
	Particle::Particle(): size(1.0f), m_span(1.0f), m_curAge(0.0f)
	{
	}

	void Particle::step(float dt)
	{
		m_curAge += dt;
	}

	bool Particle::isDead()
	{
		return m_curAge >= m_span;
	}
}
