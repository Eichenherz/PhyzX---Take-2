#include "Particle_Link.h"
#include "PX_Particle.h"
#include <cassert>

PX::Link::Link()
	:
	p_A			{ nullptr },
	p_B			{ nullptr },
	impulse		{ 0.0f }

{}

void PX::Link::Init( Particle* a, Particle* b, const Vec2& anchorA, const Vec2& anchorB )
{
	p_A = a;
	p_B = b;

	loc_anchor_A = anchorA - p_A->Get_Pos();
	loc_anchor_B = anchorB - p_B->Get_Pos();
}

void PX::Rod::Solve()
{
	const Vec2 pos_A = p_A->Get_Pos();
	const Vec2 pos_B = p_B->Get_Pos();
	Vec2 dir = pos_B - pos_A;

	// For pos correction
	const auto len = dir.GetLength();
	/*******************/

	dir.Normalize();

	const Scalar inv_mass_A = p_A->Get_Inv_Mass();
	const Scalar inv_mass_B = p_B->Get_Inv_Mass();

	const Scalar inv_mass = inv_mass_A + inv_mass_B;
	const Scalar eff_mass = (inv_mass !=  Scalar(0) ) ?
							Scalar(1)/inv_mass: 
							Scalar(0);

	const Vec2 vel_A = p_A->Get_Vel();
	const Vec2 vel_B = p_B->Get_Vel();

	const Scalar jv = dir.dot( vel_B - vel_A );
	const Scalar lagrange_mul = -eff_mass * ( jv );
	const Vec2 P = dir * lagrange_mul;

	impulse += lagrange_mul;
	p_A->Apply_Impulse( -P );
	p_B->Apply_Impulse( P );

	// Apply position correction;
	const Scalar pos_err = len - rod_length;
	const Vec2	err_P = -dir * pos_err * eff_mass;

	const Vec2 new_pos_A = pos_A - err_P * inv_mass_A;
	const Vec2 new_pos_B = pos_B + err_P * inv_mass_B;

	p_A->Set_Pos( new_pos_A );
	p_B->Set_Pos( new_pos_B );
}

void PX::Cable::Solve()
{
	const Vec2 pos_A = p_A->Get_Pos();
	const Vec2 pos_B = p_B->Get_Pos();
	Vec2 dir = pos_B - pos_A;

	const auto len = dir.GetLength();

	const Scalar pos_err = len - cable_length;
	if ( pos_err <= Scalar( 0 ) ) return;


	dir.Normalize();

	const Scalar inv_mass_A = p_A->Get_Inv_Mass();
	const Scalar inv_mass_B = p_B->Get_Inv_Mass();

	const Scalar inv_mass = inv_mass_A + inv_mass_B;
	const Scalar eff_mass = ( inv_mass != Scalar( 0 ) ) ?
		Scalar( 1 ) / inv_mass :
		Scalar( 0 );

	const Vec2 vel_A = p_A->Get_Vel();
	const Vec2 vel_B = p_B->Get_Vel();

	const Scalar jv = dir.dot( vel_B - vel_A );
	const Scalar lagrange_mul = -eff_mass * ( jv );
	const Vec2 P = dir * lagrange_mul;

	impulse += lagrange_mul;
	p_A->Apply_Impulse( -P );
	p_B->Apply_Impulse( P );

	// Apply position correction;
	const Vec2	err_P = -dir * pos_err * eff_mass;

	const Vec2 new_pos_A = pos_A - err_P * inv_mass_A;
	const Vec2 new_pos_B = pos_B + err_P * inv_mass_B;

	p_A->Set_Pos( new_pos_A );
	p_B->Set_Pos( new_pos_B );
}

void PX::Spring::Solve()
{
	Scalar inv_mass = p_A->Get_Inv_Mass() + p_B->Get_Inv_Mass();
	Scalar eff_mass = Scalar( 1 ) / inv_mass;
	
	Vec2 dir = p_B->Get_Pos() - p_A->Get_Pos();	

	// Debug
	C = dir.GetLength();


	const Scalar pos_err = dir.GetLength() - rest_length;

	// Compute soft constraint params
	const Scalar omega = CONSTANTS::TWO_PI * freq;
	const Scalar damp_coef = Scalar( 2 ) * damping_ratio * eff_mass * omega;
	const Scalar spring_coef = eff_mass * omega * omega;

	const Scalar gamma = Scalar( 1 ) / ( dt * ( damp_coef + dt * spring_coef ) );
	const Scalar beta = pos_err * dt * spring_coef * gamma;
	
	inv_mass += gamma;
	eff_mass = Scalar( 1 ) / inv_mass;
	/********************************/

	dir.Normalize();

	const Scalar jv = dir.dot( p_B->Get_Vel() - p_A->Get_Vel() );
	const Scalar lagrange_mul = -eff_mass * ( jv + beta + gamma * impulse );
	const Vec2 P = dir * lagrange_mul;

	impulse += lagrange_mul;
	p_A->Apply_Impulse( -P );
	p_B->Apply_Impulse( P );

	// No ? pos correction for springs
	const Vec2 err_P = -dir * ( pos_err * eff_mass );
	const Vec2 new_pos_A = p_A->Get_Pos() - err_P * p_A->Get_Inv_Mass();
	const Vec2 new_pos_B = p_B->Get_Pos() + err_P * p_B->Get_Inv_Mass();

	p_A->Set_Pos( new_pos_A );
	p_B->Set_Pos( new_pos_B );
}

void PX::Spring::Set_Timestep( float _dt )
{
	dt = _dt;
}