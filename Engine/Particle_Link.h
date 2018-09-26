#pragma once

#include "PX_Math.h"

namespace PX
{
	class Particle;

	struct Link
	{
	public:
						Link();
		void			Init( Particle* a, Particle* b, const Vec2& anchor );
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
		void Set_Timestep( float _dt ); 
		
	public:
		float rest_length;
		float freq; // in Hz // can't be zero;
		float damping_ratio;
		float dt; 
	};

	struct XPBD_Link : public Link
	{
		void Solve();
		void Set_Timestep( float _dt );
	public:
		float	dt;
		float	rest_length;
		float	freq;
		Vec2	last_pos = { 0.0f,0.0f };
		Scalar	lambda = 0.0f;
	};
}