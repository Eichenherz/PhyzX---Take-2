#pragma once
#include "Matrix2.h"
#include "PX_Math.h"

struct PX_OBB
{
	
	FVec2		half_lengths;
	FVec2		center;
	RotMtrx2	orientation;


	PX_OBB( const FVec2& pos, float width, float height,
			const Radians& theta = Radians { 0.0f } );

};

class PX_Box_Shape 
{
public:
					PX_Box_Shape( const FVec2& pos, float width, float height );

	const auto&	Center() const;
	void			Transform( const struct PX_Pose_Data& pose );
	void			Draw( class Graphics& gfx, class Color c ) const;
	void			Draw_test( class Graphics& gfx, class Color c ) const;


	PX_OBB			OBB;
private:
	
};
