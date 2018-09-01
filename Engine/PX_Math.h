#pragma once

#include <cmath>
#include "Matrix2.h"
#include <array>

namespace PX
{
	using Scalar = float;

	namespace CONSTANTS {
		constexpr Scalar E( 2.71828182845904523536 );
		constexpr Scalar LOG2E( 1.44269504088896340736 );
		constexpr Scalar LOG10E( 0.434294481903251827651 );
		constexpr Scalar LN2( 0.693147180559945309417 );
		constexpr Scalar LN10( 2.30258509299404568402 );
		constexpr Scalar PI( 3.14159265358979323846 );
		constexpr Scalar TWO_PI( 6.283185307179586 );
		constexpr Scalar PI_OVER_2( 1.57079632679489661923 );
		constexpr Scalar PI_OVER_4( 0.785398163397448309616 );
		constexpr Scalar SQRT2( 1.41421356237309504880 );
		constexpr Scalar SQRT1_2( 0.707106781186547524401 );
		constexpr Scalar DGS_360( 360.0 );
	};


	struct Radians
	{
		Scalar rads;

		Radians() = default;
		Radians( Scalar rads );
		Radians( const Radians& r );
		~Radians() = default;
		Radians&	operator=( const Radians& r );
		Radians		operator+( const Radians& r ) const;
		Radians&	operator+=( const Radians& r );

		//To be added as needed
		void		Normalize();
	};

	class Vec2
	{
	public:
		Vec2() = default;
		Vec2( Scalar x_in, Scalar y_in );

		Vec2	operator-() const;
		Vec2	operator+( const Vec2& rhs ) const;
		Vec2&	operator+=( const Vec2& rhs );
		Vec2	operator-( const Vec2& rhs ) const;
		Vec2&	operator-=( const Vec2& rhs );
		Vec2	operator*( Scalar rhs ) const;
		Vec2&	operator*=( Scalar rhs );
		Vec2	operator/( Scalar rhs ) const;
		Vec2&	operator/=( Scalar rhs );
		Scalar	dot( const Vec2& rhs ) const;
		// Pseudo 2d cross prod
		Scalar perp_dot( const Vec2& rhs ) const;

		Scalar	GetLength() const;
		Scalar	GetLengthSq() const;
		Vec2&	Normalize();
		Vec2	GetNormalized() const;
		Vec2&	Make_Perp();
		Vec2	GetPerp() const;

	public:
		Scalar x, y;

	};
}

