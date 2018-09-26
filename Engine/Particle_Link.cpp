#include "Particle_Link.h"
#include "PX_Particle.h"
#include <cassert>
#include "ChiliWin.h"
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <numeric>

PX::Link::Link()
	:
	p_A			{ nullptr },
	p_B			{ nullptr },
	acc_impulse { 0.0f,0.0f }
{}

void PX::Link::Init( Particle* a, Particle* b, const Vec2& anchor )
{
	p_A = a;
	p_B = b;

	loc_anchor_A = anchor - p_A->Get_Pos();
	loc_anchor_B = anchor - p_B->Get_Pos();
}

void PX::Rod::Solve()
{
	// Compute eff_mass
	const Scalar inv_mass_A = p_A->Get_Inv_Mass();
	const Scalar inv_mass_B = p_B->Get_Inv_Mass();

	const Scalar inv_mass = inv_mass_A + inv_mass_B;
	const Scalar eff_mass = ( inv_mass != Scalar( 0 ) ) ?
							Scalar( 1 ) / inv_mass :
							Scalar( 0 );
	/*******************/

	const Vec2 pos_A = p_A->Get_Pos();
	const Vec2 pos_B = p_B->Get_Pos();
	Vec2 dir = pos_B - pos_A;

	// For pos correction
	 const auto len = dir.GetLength();
	/*******************/

	dir.Normalize();
	const Vec2 vel_A = p_A->Get_Vel();
	const Vec2 vel_B = p_B->Get_Vel();

	const Scalar jv = dir.dot( vel_B - vel_A );
	const Scalar lagrange_mul = -eff_mass * ( jv );
	const Vec2 P = dir * lagrange_mul;

	acc_impulse += P;
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

	acc_impulse += P;
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

	// Compute soft constraint params
	const auto len = dir.GetLength();
	const Scalar pos_err = dir.GetLength() - rest_length;

	const Scalar omega = CONSTANTS::TWO_PI * freq;
	const Scalar damp_coef = Scalar( 2 ) * damping_ratio * eff_mass * omega;
	const Scalar spring_coef = eff_mass * omega * omega;

	const Scalar gamma = Scalar( 1 ) / ( dt * ( damp_coef + dt * spring_coef ) );
	const Scalar beta = pos_err * dt * spring_coef * gamma;

	dir.Normalize();
	const Vec2 bias = dir * pos_err * dt * spring_coef * gamma;
	
	inv_mass += gamma;
	eff_mass = Scalar( 1 ) / inv_mass;
	/********************************/

	

	const Vec2 vel_A = p_A->Get_Vel();
	const Vec2 vel_B = p_B->Get_Vel();

	const auto acc_impl = -acc_impulse.GetLength();

	const Scalar jv = dir.dot( vel_B - vel_A );
	const Scalar lagrange_mul = -eff_mass * ( jv  + beta + gamma * acc_impl );
	const Vec2 P = dir * lagrange_mul;

	acc_impulse += P;


	p_A->Apply_Impulse( -P );
	p_B->Apply_Impulse( P );

	// No pos correction for springs !

	std::wstring debug_data;
	std::wostringstream oss( debug_data );
	oss << L"C" << pos_err << L"\n";
	oss << L"K" << spring_coef << L"\n";
	oss << L"len" << len << L"\n";
	oss << L"P" << acc_impl << L"\n";
	oss << L"Gamma" << gamma << L"\n";
	oss << L"Beta" << beta << L"\n";
	debug_data = std::move( oss.str() );


	std::for_each( std::istream_iterator<std::wstring, wchar_t>( std::wistringstream( debug_data ) ),
				   std::istream_iterator<std::wstring, wchar_t>(),
				   [] ( const std::wstring& s )
				   {
					   OutputDebugString( s.c_str() );
					   OutputDebugString( L"\n" );
				   } );
}

void PX::Spring::Set_Timestep( float _dt )
{
	dt = _dt;
}

void PX::XPBD_Link::Solve()
{
	// Compute eff_mass
	Scalar inv_mass = p_A->Get_Inv_Mass() + p_B->Get_Inv_Mass();
	Scalar eff_mass = Scalar( 1 ) / inv_mass;
						
	// Compute compliance
	const Scalar omega = CONSTANTS::TWO_PI * freq;
	const Scalar spring_coef = eff_mass * omega * omega;

	const Scalar compliance = Scalar( 1 )/ (dt * dt * spring_coef);

	inv_mass += compliance;
	eff_mass = Scalar( 1 ) / inv_mass;


	const Vec2 posA = p_A->Get_Pos();
	const Vec2 dir = p_B->Get_Pos() - posA;

	// Compute d_lambda
	const Scalar distance = dir.GetLength();
	const Scalar C = distance - rest_length;
	const Scalar d_lambda = -eff_mass * ( C + compliance * lambda );
	const Scalar EPSILON = std::numeric_limits<Scalar>::epsilon();
	const Vec2 pos_correction = dir * ( d_lambda / ( distance + EPSILON ));

	lambda += d_lambda;

	const Vec2 new_pos = posA - pos_correction * inv_mass;
	p_A->Set_Pos( new_pos );


	std::wstring debug_data;
	std::wostringstream oss( debug_data );
	oss << L"C" << C << L"\n";
	/*oss << L"len" << len << L"\n";
	oss << L"Pvec" << acc_impl << L"\n";
	oss << L"Pscalar" << scalar_impl << L"\n";
	oss << L"Gamma" << gamma << L"\n";
	oss << L"Beta" << beta << L"\n";*/
	debug_data = std::move( oss.str() );


	std::for_each( std::istream_iterator<std::wstring, wchar_t>( std::wistringstream( debug_data ) ),
				   std::istream_iterator<std::wstring, wchar_t>(),
				   [] ( const std::wstring& s )
				   {
					   OutputDebugString( s.c_str() );
					   OutputDebugString( L"\n" );
				   } );
}

void PX::XPBD_Link::Set_Timestep( float _dt )
{
	dt = _dt;
}
