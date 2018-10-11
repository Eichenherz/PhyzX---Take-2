#include "PX_Math.h"

/*
//===========================
//
//			RADIANS
//
//===========================
*/


Radians::Radians( Scalar rds )
	:
	rads { rds }
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
	if ( std::fabs( rads ) >= CONSTANTS::TWO_PI )
	{
		rads -= std::copysign( CONSTANTS::TWO_PI, rads );
	}
}
//===========================//


/*
//===========================
//
//			VEC2
//
//===========================
*/

Vec2::Vec2( Scalar x_in, Scalar y_in )
	:
	x { x_in },
	y { y_in }
{}

Vec2 Vec2::operator-() const
{
	return Vec2( -x, -y );
}

Vec2 Vec2::operator+( const Vec2 & rhs ) const
{
	return Vec2( *this ) += rhs;
}

Vec2 & Vec2::operator+=( const Vec2 & rhs )
{
	x += rhs.x;
	y += rhs.y;
	return *this;
}

Vec2 Vec2::operator-( const Vec2 & rhs ) const
{
	return Vec2( *this ) -= rhs;
}

Vec2 & Vec2::operator-=( const Vec2 & rhs )
{
	x -= rhs.x;
	y -= rhs.y;
	return *this;
}

Vec2 Vec2::operator*( Scalar rhs ) const
{
	return Vec2( *this ) *= rhs;
}

Vec2 & Vec2::operator*=( Scalar rhs )
{
	x *= rhs;
	y *= rhs;
	return *this;
}

Vec2 Vec2::operator/( Scalar rhs ) const
{
	return Vec2( *this ) /= rhs;
}

Vec2 & Vec2::operator/=( Scalar rhs )
{
	x /= rhs;
	y /= rhs;
	return *this;
}

Scalar Vec2::dot( const Vec2 & rhs ) const
{
	return this->x * rhs.x + this->y*rhs.y;
}

Scalar Vec2::perp_dot( const Vec2 & rhs ) const
{
	const auto perp_this = this->GetPerp();
	return perp_this.dot( rhs );
}

Scalar Vec2::GetLength() const
{
	return std::sqrt( GetLengthSq() );
}

Scalar Vec2::GetLengthSq() const
{
	return x * x + y * y;
}

Vec2 & Vec2::Normalize()
{
	return *this = GetNormalized();
}

Vec2 Vec2::GetNormalized() const
{
	const Scalar len = GetLength();
	if ( len != Scalar( 0 ) )
	{
		return *this / len;
	}
	return *this;
}

Vec2 & Vec2::Make_Perp()
{
	return *this = GetPerp();
}

Vec2 Vec2::GetPerp() const
{
	return { -this->y, this->x };
}



