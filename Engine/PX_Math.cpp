#include "PX_Math.h"

using namespace CONSTANTS;

Radians::Radians( Scalar rads )
	:
	rads { rads }
{
	Normalize();
}

Radians::Radians( const Radians & r )
	:
	rads { r.rads }
{}

Radians & Radians::operator=( const Radians & r )
{
	this->rads = r.rads;
	return *this;
}

Radians Radians::operator+( const Radians & r ) const
{
	return Radians( rads + r.rads );
}

Radians& Radians::operator+=( const Radians & r )
{
	this->rads += r.rads;
	Normalize();
	return *this;
}

void Radians::Normalize()
{
	if ( std::fabs( rads ) >= TWO_PI )
	{
		rads -= std::copysign( TWO_PI, rads );
	}
}

Scalar Point_Point_Distance_Sq( const IVec2 & p1, const IVec2 & p2 )
{
	auto v = ( p1 - p2 );
	return Dot_Prod( v, v );
}

bool Bias_Greater_Than( Scalar a, Scalar b )
{
	constexpr Scalar relative_bias = 0.95f;
	constexpr Scalar absolute_bias = 0.01f; 

	// >= instead of > for NaN comparison safety
	return a >= b * relative_bias + a * absolute_bias;
}


