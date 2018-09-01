//#include "Matrix2.h"
//#include "PX_Math.h"
//#include <algorithm>
//#include <limits>
//
//Matrix2::Matrix2( float a_11, float a_12, float a_21, float a_22 )
//	:
//	a11 { a_11 },
//	a12 { a_12 },
//	a21 { a_21 },
//	a22 { a_22 }
//{}
//
//Matrix2::Matrix2( const Radians & r )
//{
//	auto c = cos( r.rads );
//	auto s = sin( r.rads );
//
//	a11 =  c;
//	a12 = -s;
//	a21 =  s;
//	a22 =  c;
//}
//
//Matrix2 Matrix2::operator*( const Matrix2 & rhs ) const
//{
//	return Matrix2( a11 * rhs.a11 + a12 * rhs.a21, a11 * rhs.a12 + a12 * rhs.a22,
//					a21 * rhs.a11 + a22 * rhs.a21, a21 * rhs.a12 + a22 * rhs.a22 );
//}
//
//Matrix2& Matrix2::operator*=( const Matrix2 & rhs )
//{
//	a11 = a11 * rhs.a11 + a12 * rhs.a21;
//	a12 = a11 * rhs.a12 + a12 * rhs.a22;
//	a21 = a21 * rhs.a11 + a22 * rhs.a21;
//	a22 = a21 * rhs.a12 + a22 * rhs.a22;
//
//	return *this;
//}
//
//Matrix2& Matrix2::invert()
//{
//	std::swap( a12, a21 );
//
//	return *this;
//}
//
//Matrix2 Matrix2::inverse() const
//{
//	return Matrix2( *this ).invert();
//}
//
//Matrix2& Matrix2::make_abs()
//{
//	for ( auto& r : a )
//	{
//		std::for_each( r.begin(), r.end(), 
//					   [] ( float& comp ) 
//					   {
//						   comp = std::fabs( comp ) + 
//								  std::numeric_limits<float>::epsilon();
//					   } );
//	}
//
//	return *this;
//}
//
//Matrix2 Matrix2::abs() const
//{
//	return Matrix2( *this ).make_abs();
//}