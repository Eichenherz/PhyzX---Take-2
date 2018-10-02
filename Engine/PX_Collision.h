#pragma once

#include "PX_Math.h"

namespace PX
{
	class Particle;

	struct Manifold
	{
		// Not used , at least yet
		Scalar		depth;
		Vec2		normal;
		Particle*	p_A = nullptr;
		Particle*	p_B = nullptr;
	};

	void Detect_Particle_Collision( Manifold & m, const Particle& c0, const Particle& c1 );
	void Solve_Particle_Collision( const Manifold& m );
}