#pragma once

#include "PX_Math.h"
#include <vector>

class Graphics;

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
		void	Apply_Impulse( const Vec2& p );
		void	Clear_Forces();

		Scalar	Get_Gravity() const;
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
		bool static_particle = false;
	private:
		Scalar	gravity = 10.0f;
		Scalar	mass;
		Scalar	inv_mass = 0.0f;
		Scalar	damp;

		Vec2	forces = Vec2 { 0.0f,0.0f };
		Vec2	vel;
		Vec2	pos;
	};
}

namespace PX
{
	struct Particle_Force_Generator
	{
		virtual void Update_Force( Particle& p, Scalar dt ) const = 0;
	};

	struct Basic_Spring : public Particle_Force_Generator
	{
	public:
		Basic_Spring( const Particle& other_end, Scalar spring_coef, Scalar spring_len );

		virtual void Update_Force( Particle& p, Scalar dt ) const override;

	protected:
		Scalar			spring_const;
		Scalar			spring_length;
		const Particle&	other;
	};

	struct Anchored_Spring : public Particle_Force_Generator
	{
	public:
		Anchored_Spring( const Vec2& other_end, Scalar spring_coef, Scalar spring_len );

		virtual void Update_Force( Particle& p, Scalar dt ) const override;

	protected:
		Scalar		spring_const;
		Scalar		spring_length;
		const Vec2&	other;
	};

	struct Basic_Bungee : public Basic_Spring
	{
		Basic_Bungee( const Particle& other_end, Scalar spring_coef, Scalar spring_len );
		void Update_Force( Particle& p, Scalar dt ) const override;
	};


	struct Anchored_Bungee : public Anchored_Spring
	{
		Anchored_Bungee( const Vec2& other_end, Scalar spring_coef, Scalar spring_len );
		void Update_Force( Particle& p, Scalar dt ) const override;
	};
}
namespace PX
{ 
	class Particle_Force_Registry // Might implement it diferently
	{
	public:
		void Insert( Particle& p, Particle_Force_Generator* fg );
		void Erease( Particle& p, Particle_Force_Generator* fg );//Will be impl later
		void Clear();
		void Update_Forces( Scalar dt );

	private:
		struct Reg_Entry
		{
			Particle&					p_particle;
			Particle_Force_Generator*	p_force_gen; // can use reference instead
		};
		using Registry = std::vector<Reg_Entry>;

		Registry reg;
	};
}