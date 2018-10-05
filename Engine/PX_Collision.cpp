#include "PX_Collision.h"
#include "PX_Particle.h"
#include <algorithm>


inline bool PX::Detect_Collision( const Particle & p0, const Particle & p1 )
{
	// Paricle radius is 4.0f
	constexpr float TWO_RADIUS_SQ = 64.0f;
	const auto dist = ( p1.Get_Pos() - p0.Get_Pos() ).GetLengthSq();

	return dist < TWO_RADIUS_SQ;
}

void PX::Broad_Phase( const std::vector<PX::Particle>& particles, const std::vector<Wall>& walls,
					  std::vector<std::unique_ptr<Manifold>>& manifolds )
{
	const auto size = ( particles.size() + walls.size() ) * particles.size();
	manifolds.reserve( size );

	// O(n^2) broadpahse
	for ( auto I = particles.cbegin(); I != std::prev( particles.cend() ); ++I )
	{
		for ( auto J = std::next(I); J != particles.cend(); ++J )
		{
			const auto contact = Detect_Collision( *I, *J );
			if ( contact )
			{
				manifolds.emplace_back( std::make_unique<Particle_Manifold>(*I,*J) );
			}
		}
	}

	for ( const auto& wall : walls )
	{
		for ( const auto& particle : particles )
		{
			auto m = std::make_unique<Border_Manifold>( particle, wall );
			// Pre update to prevent unnecessary addition and deletion of border manifolds
			// since circle-wall intersection is not as trivial as circle - circle
			if ( m->Update() )
			{
				manifolds.emplace_back( std::move( m ) );
			}
		}
	}

	manifolds.shrink_to_fit();
}

void PX::Filter_Contacts( std::vector<std::unique_ptr<Manifold>>& manifolds )
{
	const auto filter = [] ( const std::unique_ptr<Manifold>& m ) -> bool
	{
		return !m->Update();
	};
	manifolds.erase( std::remove_if( manifolds.begin(), manifolds.end(), filter ), manifolds.end() );
	manifolds.shrink_to_fit();
}

PX::Particle_Manifold::Particle_Manifold( const Particle & p0, const Particle & p1 )
{
	// Manifold's solve will modify the particle
	p_A = const_cast<Particle*>( std::addressof( p0 ) );
	p_B = const_cast<Particle*>( std::addressof( p1 ) );
}

bool PX::Particle_Manifold::Update()
{
	// Set a pseudo-fixture for mass-pts.
	constexpr float PARTICLE_RADIUS = 4.0f;

	Vec2 d_pos = p_B->Get_Pos() - p_A->Get_Pos();

	if ( d_pos.GetLengthSq() > 4.0f * PARTICLE_RADIUS * PARTICLE_RADIUS )
	{
		return false;
	}

	const Scalar dist = d_pos.GetLength();
	// Mass-point so contact pt is its own center.
	if ( dist != Scalar( 0 ) )
	{
		normal = d_pos / dist;
	}
	else
	{
		// Pick random but consistent values if overlapping
		normal = Vec2 { 1.0f,0.0f };
	}

	return true;
}

void PX::Particle_Manifold::Solve()
{
	// Might be removed later
	if ( ( p_A == nullptr ) && ( p_B == nullptr ) ) return;

	const Vec2 v_rel = p_B->Get_Vel() - p_A->Get_Vel();
	const Scalar contact_vel = v_rel.dot( normal );

	// If vel is separating do nothing
	if ( contact_vel > Scalar( 0 ) ) return;

	const auto restitution = std::min( p_A->Get_Restitution(), p_B->Get_Restitution() );
	const Scalar eff_mass = Scalar( 1 ) / ( p_A->Get_Inv_Mass() + p_B->Get_Inv_Mass() );
	const Scalar lambda = -eff_mass * ( Scalar( 1 ) + restitution ) * contact_vel;

	const Vec2 P = normal * lambda;
	p_A->Apply_Impulse( -P );
	p_B->Apply_Impulse( P );
}

PX::Border_Manifold::Border_Manifold( const Particle & p, const Wall & w )
{
	normal = w.normal;
	VA = w.A;
	VB = w.B;
	p_A = const_cast<Particle*>( std::addressof( p ) );
}

bool PX::Border_Manifold::Update() 
{
	// Set a pseudo-fixture for mass-pts.
	constexpr float PARTICLE_RADIUS = 4.0f;
	constexpr float PENETRATION_THRESHOLD = 0.001f;

	// Compute displacement vector
	const Vec2 Ua = VA - p_A->Get_Pos();
	const Vec2 Uba = VB - VA;
	const float interpolant = -Ua.dot( Uba ) / Uba.dot( Uba );

	const Vec2 Ux = Ua + Uba * interpolant;

	if ( Ux.GetLengthSq() > PARTICLE_RADIUS * PARTICLE_RADIUS )
	{
		return false;
	}

	// Normal is already set
	//normal = -Ux.GetNormalized();

	const auto sep = Ux.GetLength() - PARTICLE_RADIUS;
	if ( sep > PENETRATION_THRESHOLD )
	{
		separation = sep;
	}
	
	return true;
}

void PX::Border_Manifold::Solve()
{
	//p_A->Apply_Impulse( normal * impulse );

	const Scalar contact_vel = p_A->Get_Vel().dot( normal );

	// Separating so leave alone
	if ( contact_vel > 0.01f ) return;

	// Baumgarte term
	const auto bias = separation * 0.2f / dt;
	const auto restitution = 0.0f;// p_A->Get_Restitution();
	

	Scalar lambda = -( Scalar( 1 ) + restitution ) * ( contact_vel + bias );
	// Clamp impulse
	const auto new_impl = std::max( impulse + lambda, 0.0f );
	lambda = new_impl - impulse;
	impulse = new_impl;

	const Vec2 pseudo_P = normal * lambda;
	p_A->Add_Vel( pseudo_P );

	// Pos correction
	const auto eff_mass = p_A->Get_Mass();
	const Scalar correction = std::max( separation - 0.01f, 0.0f ) * 0.2f * eff_mass;
	const Vec2 correction_P = -normal * correction;
	p_A->Add_Pos( correction_P );
}
