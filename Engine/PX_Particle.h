#pragma once

#include "PX_Math.h"
#include <vector>
#include "Colors.h"

class Graphics;


// STD PARICLE RADIUS = 4.0f
namespace PX
{
	class Particle
	{
	public:
		void	Set_Restitution( Scalar e );
		void	Set_Mass( Scalar m );
		void	Set_Damp( Scalar d );
		void	Set_Pos( const Vec2& p );
		void	Set_Vel( const Vec2& v );
		// Don't use yet;
		void	Apply_Force( const Vec2& f );
		void	Apply_Impulse( const Vec2& p );
		void	Add_Vel( const Vec2& v );
		void	Add_Pos( const Vec2& p );
		void	Clear_Forces();
		void	Apply_Gravity( Scalar dt );

		Scalar  Get_Restitution() const;
		Scalar	Get_Mass() const;
		Scalar	Get_Inv_Mass() const;
		Scalar	Get_Damp() const;
		Vec2	Get_Pos() const;
		Vec2	Get_Vel() const;
		bool	has_Finite_Mass() const;

		void	Debug_Draw( Graphics& gfx ) const;
	//private:
		// World is responsible of updating obj state
		//friend class World;

		void	Update( Scalar dt );

	private:
		Scalar	mass;
		Scalar	inv_mass = 0.0f;
		Scalar	restitution = 1.0f;
		Scalar	damp;

		Vec2	forces = Vec2 { 0.0f,0.0f };
		Vec2	vel;
		Vec2	pos;
	};
}