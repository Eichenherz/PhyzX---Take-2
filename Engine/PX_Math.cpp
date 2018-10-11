#include "PX_Math.h"

using namespace PX;

/*
//===========================
//
//			RADIANS
//
//===========================
*/


PX::Radians::Radians( Scalar rds )
	:
	rads { rds }
{
	Normalize();
}

PX::Radians::Radians( const Radians & r )
	:
	rads { r.rads }
{}

PX::Radians & PX::Radians::operator=( const Radians & r )
{
	this->rads = r.rads;
	return *this;
}

PX::Radians PX::Radians::operator+( const Radians & r ) const
{
	return Radians( rads + r.rads );
}

PX::Radians& PX::Radians::operator+=( const Radians & r )
{
	this->rads += r.rads;
	Normalize();
	return *this;
}

void PX::Radians::Normalize()
{
	if ( std::fabs( rads ) >= PX::CONSTANTS::TWO_PI )
	{
		rads -= std::copysign( PX::CONSTANTS::TWO_PI, rads );
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

Vec2 PX::Vec2::operator-() const
{
	return Vec2( -x, -y );
}

Vec2 Vec2::operator+( const Vec2 & rhs ) const
{
	return Vec2( *this ) += rhs;
}

Vec2 & PX::Vec2::operator+=( const Vec2 & rhs )
{
	x += rhs.x;
	y += rhs.y;
	return *this;
}

Vec2 PX::Vec2::operator-( const Vec2 & rhs ) const
{
	return Vec2( *this ) -= rhs;
}

Vec2 & PX::Vec2::operator-=( const Vec2 & rhs )
{
	x -= rhs.x;
	y -= rhs.y;
	return *this;
}

Vec2 PX::Vec2::operator*( Scalar rhs ) const
{
	return Vec2( *this ) *= rhs;
}

Vec2 & PX::Vec2::operator*=( Scalar rhs )
{
	x *= rhs;
	y *= rhs;
	return *this;
}

Vec2 PX::Vec2::operator/( Scalar rhs ) const
{
	return Vec2( *this ) /= rhs;
}

Vec2 & PX::Vec2::operator/=( Scalar rhs )
{
	x /= rhs;
	y /= rhs;
	return *this;
}

Scalar PX::Vec2::dot( const Vec2 & rhs ) const
{
	return this->x * rhs.x + this->y*rhs.y;
}

Scalar PX::Vec2::perp_dot( const Vec2 & rhs ) const
{
	const auto perp_this = this->GetPerp();
	return perp_this.dot( rhs );
}

Scalar PX::Vec2::GetLength() const
{
	return std::sqrt( GetLengthSq() );
}

Scalar PX::Vec2::GetLengthSq() const
{
	return x * x + y * y;
}

Vec2 & PX::Vec2::Normalize()
{
	return *this = GetNormalized();
}

Vec2 PX::Vec2::GetNormalized() const
{
	const Scalar len = GetLength();
	if ( len != Scalar( 0 ) )
	{
		return *this / len;
	}
	return *this;
}

Vec2 & PX::Vec2::Make_Perp()
{
	return *this = GetPerp();
}

Vec2 PX::Vec2::GetPerp() const
{
	return { -this->y, this->x };
}



