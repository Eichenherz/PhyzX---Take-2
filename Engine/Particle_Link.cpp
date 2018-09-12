#include "Particle_Link.h"
#include "PX_Particle.h"

PX::Link::Link()
	:
	p_A			{ nullptr },
	p_B			{ nullptr },
	acc_impulse { 0.0f,0.0f },
	bias_factor	{ 0.2f },
	softness	{ 0.0f }

{}

void PX::Link::Init( Particle* a, Particle* b, const Vec2& anchorA, const Vec2& anchorB )
{
	p_A = a;
	p_B = b;

	loc_anchor_A = anchorA - p_A->Get_Pos();
	loc_anchor_B = anchorB - p_B->Get_Pos();
}

void PX::Rod::SolveVel()
{
	const Vec2 pos_A = p_A->Get_Pos();
	const Vec2 pos_B = p_B->Get_Pos();
	Vec2 dir = pos_B - pos_A;


	dir.Normalize();
	Vec2 vel_A = p_A->Get_Vel();
	Vec2 vel_B = p_B->Get_Vel();

	const Scalar inv_mass_A = p_A->Get_Inv_Mass();
	const Scalar inv_mass_B = p_B->Get_Inv_Mass();

	const Scalar inv_mass = inv_mass_A + inv_mass_B;
	const Scalar eff_mass = (inv_mass !=  Scalar(0) ) ?
							Scalar(1)/inv_mass: 
							Scalar(0);

	const Scalar jv = dir.dot( vel_B - vel_A );
	const Scalar lagrange_mul = -eff_mass * ( jv );
	const Vec2 P = dir * lagrange_mul;

	acc_impulse += P;
	p_A->Apply_Impulse( -P );
	p_B->Apply_Impulse( P );
}

void PX::Rod::SolvePos()
{
}