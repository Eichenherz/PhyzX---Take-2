#include "PX_Collision.h"
#include "PX_Particle.h"

void PX::Detect_Particle_Collision( Manifold & m, const Particle& p0, const Particle& p1 )
{
	// Set a pseudo-fixture for mass-pts.
	constexpr float PARTICLE_RADIUS = 4.0f;

	Vec2 d_pos = p1.Get_Pos() - p0.Get_Pos();

	if ( d_pos.GetLengthSq() > 4.0f * PARTICLE_RADIUS * PARTICLE_RADIUS )
	{
		p0.c = Colors::Red;
		return;
	}
	p0.c = Colors::Blue;


	const Scalar dist = d_pos.GetLength();
	// Mass-point so contact pt is its own center.
	if ( dist != Scalar( 0 ) )
	{
		m.depth = 2 * PARTICLE_RADIUS - dist;
		m.normal = d_pos / dist;
	}
	else
	{
		// Pick random but consistent values if overlapping
		m.depth = PARTICLE_RADIUS;
		m.normal = Vec2 { 1.0f,0.0f };
	}
	// OUCH !!! But manifold data is used to actually modify the particles.
	m.p_A = const_cast<Particle*>( std::addressof( p0 ) );
	m.p_B = const_cast<Particle*>( std::addressof( p1 ) );
}

void PX::Solve_Particle_Collision( const Manifold & m )
{
	if ( ( m.p_A == nullptr ) && ( m.p_B == nullptr ) ) return;

	const Vec2 v_rel = m.p_B->Get_Vel() - m.p_A->Get_Vel();
	const Scalar contact_vel = v_rel.dot( m.normal );

	// If vel is separating do nothing
	if ( contact_vel > Scalar( 0 ) ) return;

	const auto restitution = std::min( m.p_A->Get_Restitution(), m.p_B->Get_Restitution() );
	const Scalar eff_mass = Scalar( 1 ) / ( m.p_A->Get_Inv_Mass()+ m.p_B->Get_Inv_Mass() );
	const Scalar lambda = - eff_mass * ( Scalar( 1 ) + restitution ) * contact_vel;

	const Vec2 P = m.normal * lambda;
	m.p_A->Apply_Impulse( -P );
	m.p_B->Apply_Impulse(  P );
}
