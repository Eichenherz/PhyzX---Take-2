#include "PX_Particle.h"
#include <cassert>
#include "Graphics.h"
using namespace PX;


void PX::Particle::Set_Restitution( Scalar e )
{
	if ( !has_Finite_Mass() ) return;
	restitution = e;
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
	if ( !has_Finite_Mass() ) return;

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
	vel += p * inv_mass;
}

void PX::Particle::Add_Vel( const Vec2 & v )
{
	if ( !has_Finite_Mass() ) return;
	vel += v;
}

void PX::Particle::Add_Pos( const Vec2 & p )
{
	if ( !has_Finite_Mass() ) return;
	pos += p;
}

void PX::Particle::Clear_Forces()
{
	forces = Vec2 { 0.0f,0.0f };
}

void PX::Particle::Apply_Gravity( Scalar dt )
{
	vel += Vec2 { 0.0f,100.0f } *dt;
}

Scalar PX::Particle::Get_Restitution() const
{
	return restitution;
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
	constexpr float		radius = 4.0f;
	constexpr size_t	precision = 26; // do not change !!!
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
	if ( !has_Finite_Mass() ) return;

	vel += forces * inv_mass * dt;
	vel *= damp;
	pos += vel * dt;

	Clear_Forces();
}

