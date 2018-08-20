#pragma once

#include <cmath>
#include <cassert>
#include "Vec2.h"


template<typename T>
class Vec3_
{
public:
	Vec3_() = default;
	Vec3_( T x_in, T y_in, T z_in )
		:
		x( x_in ),
		y( y_in ),
		z( z_in )
	{}
	template<typename S>
	explicit Vec3_( const Vec3_<S>& src )
		:
		x( (T) src.x ),
		y( (T) src.y ),
		z( (T) src.z )
	{}
	Vec3_	operator+( const Vec3_& rhs ) const
	{
		return Vec3_( *this ) += rhs;
	}
	Vec3_&	operator+=( const Vec3_& rhs )
	{
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;
		return *this;
	}
	Vec3_	operator-( const Vec3_& rhs ) const
	{
		return Vec3_( *this ) -= rhs;
	}
	Vec3_&	operator-=( const Vec3_& rhs )
	{
		x -= rhs.x;
		y -= rhs.y;
		z -= rhs.z;
		return *this;
	}
	Vec3_	operator*( T rhs ) const
	{
		return Vec3_( *this ) *= rhs;
	}
	Vec3_&	operator*=( T rhs )
	{
		x *= rhs;
		y *= rhs;
		z *= rhs;
		return *this;
	}
	Vec3_	operator/( T rhs ) const
	{
		return Vec3_( *this ) /= rhs;
	}
	Vec3_&	operator/=( T rhs )
	{
		x /= rhs;
		y /= rhs;
		z /= rhs;
		return *this;
	}
	Vec3_	operator-() const
	{
		return Vec3_( -x, -y, -z );
	}
	T		GetLength() const
	{
		return (T) std::sqrt( GetLengthSq() );
	}
	T		GetLengthSq() const
	{
		return x * x + y * y + z * z;
	}
	Vec3_&	Normalize()
	{
		return *this = GetNormalized();
	}
	Vec3_	GetNormalized() const
	{
		const T len = GetLength();
		if ( len != (T) 0 )
		{
			return *this / len;
		}
		return *this;
	}

	T dot( const Vec3_& vec1, const Vec3_& vec2 )
	{
		return vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;
	}

	Vec3_ cross( const Vec3_& u, const Vec3_& v )
	{
		return {
			u.y * v.z - u.z * v.y,
			u.z * v.x - u.x * v.z,
			u.x * v.y - u.y * v.x };
	}

public:
	T x, y, z;

};

using FVec3 = Vec3_<float>;
using IVec3 = Vec3_<int>;

template<typename T>
Vec3_<T> make_homogenous( const Vec2_<T>& v )
{
	return { v.x, v.y, T( 1 ) };
}