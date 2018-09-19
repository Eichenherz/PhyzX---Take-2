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
		Vec2		loc_anchor_A;
		Vec2		loc_anchor_B;
	};

	struct Rod : public Link
	{
	public:
		void Solve() override;
		void Set_Timestep( float _dt);

	public:
		float rod_length;
		float dt;
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
		void Set_Timestep( float _dt ); 
		
	public:
		float rest_length;
		float freq; // in Hz // can't be zero;
		float damping_ratio;
		float dt; 
	};
}