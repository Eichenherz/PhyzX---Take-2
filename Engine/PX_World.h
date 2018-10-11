#pragma once

#include "PhyzX\PX_Particle.h"
#include "PhyzX\PX_Particle_Link.h"
#include "PhyzX\PX_Collision.h"
#include <vector>
#include <random>

class Graphics;

namespace PX
{
	class World
	{
	public:
		void Init_Sym( size_t size, std::mt19937& rng );
		void Apply_Mouse_Imput( const Vec2& pos );
		void Update( Scalar dt );
		void Draw( Graphics& gfx );

	private:
		std::vector<Particle> particles;
		std::vector<Wall> walls;
		std::vector<std::unique_ptr<Manifold>> manifolds;

		Rod s0;
		Rod s1;
		Rod s2;
	};
}