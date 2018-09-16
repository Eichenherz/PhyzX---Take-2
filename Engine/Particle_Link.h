#pragma once

#include "PX_Math.h"

namespace PX
{
	class Particle;

	struct Link
	{
	public:
						Link();
		void			Init( Particle* a, Particle* b, const Vec2& anchorA, const Vec2& anchorB );
		virtual void	Solve() = 0;

	public: // Will be made private in the future.
		Particle*	p_A;
		Particle*	p_B;
		Vec2		acc_impulse;
		Vec2		bias;
		Vec2		loc_anchor_A;
		Vec2		loc_anchor_B;
		float		bias_factor;
		float		softness;
	};

	struct Rod : public Link
	{
	public:
		void Solve() override;

	public:
		float rod_length;
	};

	struct Cable : public Link
	{
	public:
		void Solve() override;

	public:
		float cable_length;
	};

	struct Spring : public Link
	{
	public:
		void Solve() override;

	public:
		float rest_length;
		// bunch of other stuff
	};
}