//#pragma once
//
//#include <cassert>
//#include <array>
//
//
//class Matrix2
//{
//public:
//	union
//	{
//		struct
//		{
//			float a11, a12;
//			float a21, a22;
//		};
//
//		std::array<std::array<float, 2>, 2> a;
//	};
//
//
//
//public:
//				Matrix2() = default;
//				Matrix2( float a_11, float a_12, float a_21, float a_22 );
//				Matrix2( const struct Radians& r );
//
//	Matrix2		operator*( const Matrix2& rhs ) const;
//	Matrix2&	operator*=( const Matrix2& rhs );
//				/*
//				Matrix2		operator+ ( const Matrix2& rhs ) const;
//				Matrix2&	operator+= ( const Matrix2& rhs );
//				Matrix2		operator- ( const Matrix2& rhs ) const;
//				Matrix2&	operator-= ( const Matrix2& rhs );
//				Matrix2		operator* ( float scalar ) const;
//				Matrix2&	operator*= ( float scalar );
//				Matrix2		operator/ ( float scalar ) const;
//				Matrix2&	operator/= ( float scalar );
//				*/
//	
//
//	template<typename S>
//	Vec2_<S>	operator*( const Vec2_<S>& rhs ) const;
//	template<typename S>
//	Vec2_<S>&	operator*=( Vec2_<S>& rhs ) const;
//
//	Matrix2&	invert();
//	Matrix2		inverse() const;
//	Matrix2&	make_abs();
//	Matrix2		abs() const;
//
//	FVec2		Basis_X() const
//	{
//		return { a11, a21 };
//	}
//	FVec2		Basis_Y() const
//	{
//		return { a12, a22 };
//	}
//};
//
//using RotMtrx2 = Matrix2;
//
//template<typename S>
//Vec2_<S> Matrix2::operator*( const Vec2_<S>& rhs ) const
//{
//	assert( rhs.is_Column() );
//
//	auto rhs_x = rhs.x;
//	auto rhs_y = rhs.y;
//
//	return Vec2_<S>( S( a11 * rhs_x + a12 * rhs_y ),
//					 S( a21 * rhs_x + a22 * rhs_y ) );
//}
//
//template<typename S>
//Vec2_<S>& Matrix2::operator*=( Vec2_<S>& rhs ) const
//{
//	assert( rhs.is_Column() );
//
//	auto rhs_x = rhs.x;
//	auto rhs_y = rhs.y;
//
//	rhs.x = S( a11 * rhs_x + a12 * rhs_y );
//	rhs.y = S( a21 * rhs_x + a22 * rhs_y );
//
//	return rhs;
//}
//
///*
//Matrix2 Matrix2::operator+( const Matrix2 & rhs ) const
//{
//return Matrix2( a11 + rhs.a11, a12 + rhs.a12,
//a21 + rhs.a21, a22 + rhs.a22 );
//}
//
//Matrix2& Matrix2::operator+=( const Matrix2 & rhs )
//{
//this->a11 += rhs.a11;
//this->a12 += rhs.a12;
//this->a21 += rhs.a21;
//this->a22 += rhs.a22;
//
//return *this;
//}
//
//Matrix2 Matrix2::operator-( const Matrix2 & rhs ) const
//{
//return Matrix2( a11 - rhs.a11, a12 - rhs.a12,
//a21 - rhs.a21, a22 - rhs.a22 );
//}
//
//Matrix2& Matrix2::operator-=( const Matrix2 & rhs )
//{
//this->a11 -= rhs.a11;
//this->a12 -= rhs.a12;
//this->a21 -= rhs.a21;
//this->a22 -= rhs.a22;
//
//return *this;
//}
//
//
//
//Matrix2 Matrix2::operator*( float scalar ) const
//{
//return Matrix2( scalar * a11, scalar * a12,
//scalar * a21, scalar * a22 );
//}
//
//Matrix2& Matrix2::operator*=( float scalar )
//{
//this->a11 *= scalar;
//this->a12 *= scalar;
//this->a21 *= scalar;
//this->a22 *= scalar;
//
//return *this;
//}
//
//Matrix2 Matrix2::operator/( float scalar ) const
//{
//return Matrix2( a11 / scalar, a12 / scalar,
//a21 / scalar, a22 / scalar );
//}
//
//Matrix2& Matrix2::operator/=( float scalar )
//{
//this->a11 /= scalar;
//this->a12 /= scalar;
//this->a21 /= scalar;
//this->a22 /= scalar;
//
//return *this;
//}
//*/
//
//
//
//
//
//	