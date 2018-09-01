#include "PX_Particle.h"

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

void PX::Particle::Update( Scalar dt )
{
	vel += Vec2 { 0.0f,-1.0f } *gravity * dt;
	vel += forces * inv_mass * dt;
	vel *= damp;
	pos += vel * dt;
}

