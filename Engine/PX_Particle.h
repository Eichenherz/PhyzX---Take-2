#pragma once

#include "PX_Math.h"


namespace PX
{
	class Particle
	{
	public:
		void	Set_Gravity( Scalar g );
		void	Set_Mass( Scalar m );
		void	Set_Damp( Scalar d );
		void	Set_Pos( const Vec2& p );
		void	Set_Vel( const Vec2& v );
		void	Apply_Force( const Vec2& f );
		void	Clear_Forces();

		Scalar	Get_Gravity() const;
		Scalar	Get_Mass() const;
		Scalar	Get_Inv_Mass() const;
		Scalar	Get_Damp() const;
		Vec2	Get_Pos() const;
		Vec2	Get_Vel() const;
		bool	has_Finite_Mass() const;

	private:
		// World is responsible of updating obj state & 
		// for applying gravity.
		friend class World;

		void	Update( Scalar dt );

	private:
		Scalar	gravity = 10.0f;
		Scalar	mass;
		Scalar	inv_mass = 0.0f;
		Scalar	damp;

		Vec2	forces;
		Vec2	vel;
		Vec2	pos;
	};
}