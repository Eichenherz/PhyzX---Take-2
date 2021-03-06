#pragma once

#include "PX_Math.h"

namespace PX
{
	class Particle;

	struct Link
	{
	public:
						Link();
		void			Init( Particle* a, Particle* b );
		virtual void	Solve() = 0;

	public: // Will be made private in the future.
		Particle*	p_A;
		Particle*	p_B;
		Scalar		impulse;
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
		float freq; 
		float damping_ratio;
		float dt; 
	};
}