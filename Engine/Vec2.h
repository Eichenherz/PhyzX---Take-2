#pragma once

#include <cmath>
#include <cassert>

enum class Vec_Type
{
	COLUMN,
	ROW
};

template<typename T, Vec_Type t = Vec_Type::COLUMN>
class Vec2_
{
public:
	Vec2_() = default;
	Vec2_( T x_in, T y_in )
		:
		x( x_in ),
		y( y_in )
	{}
	template<typename S>
	explicit Vec2_( const Vec2_<S>& src )
		:
		x( (T) src.x ),
		y( (T) src.y )
	{}
	Vec2_	operator+( const Vec2_& rhs ) const
	{
		return Vec2_( *this ) += rhs;
	}
	Vec2_&	operator+=( const Vec2_& rhs )
	{
		x += rhs.x;
		y += rhs.y;
		return *this;
	}
	Vec2_	operator-( const Vec2_& rhs ) const
	{
		return Vec2_( *this ) -= rhs;
	}
	Vec2_&	operator-=( const Vec2_& rhs )
	{
		x -= rhs.x;
		y -= rhs.y;
		return *this;
	}
	Vec2_	operator*( T rhs ) const
	{
		return Vec2_( *this ) *= rhs;
	}
	Vec2_&	operator*=( T rhs )
	{
		x *= rhs;
		y *= rhs;
		return *this;
	}
	Vec2_	operator/( T rhs ) const
	{
		return Vec2_( *this ) /= rhs;
	}
	Vec2_&	operator/=( T rhs )
	{
		x /= rhs;
		y /= rhs;
		return *this;
	}
	Vec2_	operator-() const
	{
		return Vec2_( -x, -y );
	}
	T		GetLength() const
	{
		return (T) std::sqrt( GetLengthSq() );
	}
	T		GetLengthSq() const
	{
		return x * x + y * y;
	}
	Vec2_&	Normalize()
	{
		return *this = GetNormalized();
	}
	Vec2_	GetNormalized() const
	{
		const T len = GetLength();
		if ( len != (T) 0 )
		{
			return *this / len;
		}
		return *this;
	}
	Vec2_&	Make_Perp()
	{
		return *this = GetPerp();
	}
	Vec2_	GetPerp() const
	{
		return { -this->y, this->x };
	}
	
	

	bool	is_Column() const
	{
		return t == Vec_Type::COLUMN;
	}

public:
	T x, y;
	
};

using FVec2 = Vec2_<float>;
using IVec2 = Vec2_<int>;


template<class Vec1, class Vec2>
float Dot_Prod( const Vec1& vec1, const Vec2& vec2 )
{
	return float( vec1.x ) * float( vec2.x ) + float( vec1.y ) * float( vec2.y );// ouch
}

template<class Vec1, class Vec2>
float Perp_Dot_Prod( const Vec1& vec1, const Vec2& vec2 )
{
	auto perp_v1 = vec1.GetPerp();

	return perp_v1.x * vec2.x + perp_v1.y * vec2.y;
}