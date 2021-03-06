#pragma once

#include "PX_Math.h"
#include <vector>
#include <memory>

namespace PX
{
	class Particle;
	struct Wall
	{
		// End pts
		Vec2 A;
		Vec2 B;
		// Pre-def normal to avoid computation
		Vec2 normal;
	};
	struct Manifold;

	inline bool Detect_Collision( const Particle& p0, const Particle& p1 );
	inline bool Particle_Wall( const Particle& p, const Wall& w );
	void Broad_Phase( const std::vector<Particle>& particles, const std::vector<Wall>& walls,
					  std::vector<std::unique_ptr<Manifold>>& manifolds );
	void Filter_Contacts( std::vector<std::unique_ptr<Manifold>>& manifolds );

	struct Manifold
	{
	public:
		virtual bool Update() = 0;
		virtual void Solve() = 0;
		virtual void Warm_Start() = 0;

	public:
		Vec2		normal;
		Particle*	p_A;
		Scalar		separation = 0.0f;
		Scalar		impulse = 0.0f;
		float		dt;
	};

	struct Border_Manifold : public Manifold
	{
	public:
		Border_Manifold( const Particle& p, const Wall& w );
		bool Update() override;
		void Solve() override;
		void Warm_Start() override;

	public:
		Vec2 VA;
		Vec2 VB;
	};

	struct Particle_Manifold : public Manifold
	{
	public:
		Particle_Manifold( const Particle& p0, const Particle& p1 );
		bool Update() override;
		void Solve() override;
		void Warm_Start() override;

	public:
		Particle* p_B;
	};
}