#include "PX_Particle.h"
#include <cassert>
#include "Graphics.h"
using namespace PX;


void PX::Particle::Set_Gravity( Scalar g )
{
	gravity = g;
}

void PX::Particle::Set_Mass( Scalar m )
{
	mass = m;
	inv_mass = Scalar( 1 ) / m;
}

void PX::Particle::Set_Damp( Scalar d )
{
	damp = d;
}

void PX::Particle::Set_Pos( const Vec2 & p )
{
	if ( static_particle ) return;

	pos = p;
}

void PX::Particle::Set_Vel( const Vec2 & v )
{
	vel = v;
}

void PX::Particle::Apply_Force( const Vec2 & f )
{
	forces += f;
}

void PX::Particle::Apply_Impulse( const Vec2 & p )
{
	if ( static_particle ) return;

	vel += p * inv_mass;
}

void PX::Particle::Clear_Forces()
{
	forces = Vec2 { 0.0f,0.0f };
}

Scalar PX::Particle::Get_Gravity() const
{
	return gravity;
}

Scalar PX::Particle::Get_Mass() const
{
	return mass;
}

Scalar PX::Particle::Get_Inv_Mass() const
{
	return inv_mass;
}

Scalar PX::Particle::Get_Damp() const
{
	return damp;
}

Vec2 PX::Particle::Get_Pos() const
{
	return pos;
}

Vec2 PX::Particle::Get_Vel() const
{
	return vel;
}

bool PX::Particle::has_Finite_Mass() const
{
	return inv_mass != Scalar(0);
}

void PX::Particle::Debug_Draw( Graphics & gfx ) const
{
	constexpr float		radius = 20.0f;
	constexpr size_t	precision = 13; // do not change !!!
	constexpr float		step = CONSTANTS::PI / 6.0f;
	std::vector<Vec2>	vertices( precision );

	for ( size_t i = 0; i < precision; ++i )
	{
		const Vec2 vertex = Vec2 { std::cos( float( i ) * step ), std::sin( float( i ) * step ) } *radius;
		vertices [i] = std::move( vertex );
	}

	auto xform = [&] ( Vec2& vertex )
	{
		vertex += this->Get_Pos();
	};

	std::for_each( vertices.begin(), vertices.end(), xform );
	gfx.Draw_Closed_Polyline( vertices.begin(), vertices.end(), Colors::Red );
}

void PX::Particle::Update( Scalar dt )
{
	if ( static_particle ) return;

	vel += forces * inv_mass * dt;
	vel *= damp;
	pos += vel * dt;

	Clear_Forces();
}

void PX::Particle_Force_Registry::Insert( Particle & p, Particle_Force_Generator * fg )
{
	Reg_Entry r { p,fg };
	reg.emplace_back( std::move(r) );
}

void PX::Particle_Force_Registry::Erease( Particle & p, Particle_Force_Generator * fg )
{
}

void PX::Particle_Force_Registry::Clear()
{
	reg.clear();
}

void PX::Particle_Force_Registry::Update_Forces( Scalar dt )
{
	for ( auto& entry : reg )
	{
		entry.p_force_gen->Update_Force( entry.p_particle, dt );
	}
}

PX::Basic_Spring::Basic_Spring( const Particle& other_end, Scalar spring_coef, Scalar spring_len )
	:
	spring_const	{ spring_coef },
	spring_length	{ spring_len },
	other			{ other_end }
{
	assert( spring_const > Scalar( 0 ) );
	assert( spring_length > Scalar( 0 ) );
}

void PX::Basic_Spring::Update_Force( Particle & p, Scalar dt ) const
{
	auto force = p.Get_Pos() - other.Get_Pos();
	const Scalar mag = std::fabs( force.GetLength() - spring_length ) * spring_const;

	force.Normalize();
	force *= -mag;

	p.Apply_Force( force );
}

PX::Basic_Bungee::Basic_Bungee( const Particle & other_end, Scalar spring_coef, Scalar spring_len )
	:
	Basic_Spring { other_end, spring_coef, spring_len }
{}

void PX::Basic_Bungee::Update_Force( Particle & p, Scalar dt ) const
{
	const auto len = ( p.Get_Pos() - other.Get_Pos() ).GetLengthSq();
	if ( len <= spring_length * spring_length ) return;

	Basic_Spring::Update_Force( p, dt );
}

PX::Anchored_Spring::Anchored_Spring( const Vec2 & other_end, Scalar spring_coef, Scalar spring_len )
	:
	other			{ other_end },
	spring_const	{ spring_coef },
	spring_length	{ spring_len }
{
	assert( spring_const > Scalar( 0 ) );
	assert( spring_length > Scalar( 0 ) );
}

void PX::Anchored_Spring::Update_Force( Particle & p, Scalar dt ) const
{
	auto force = p.Get_Pos() - other;
	const Scalar mag = std::fabs( force.GetLength() - spring_length ) * spring_const;

	force.Normalize();
	force *= -mag;

	p.Apply_Force( force );
}
