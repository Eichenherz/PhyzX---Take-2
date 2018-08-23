#include "PX_Box_Shape.h"
#include "PX_Physical_Traits.h"
#include <math.h>
#include "Graphics.h"


PX_OBB::PX_OBB( const FVec2& pos, float width, float height, const Radians& theta )
	:
	half_lengths { width / 2, height / 2 },
	center { pos }, ////// +half_lengths },
	orientation { theta }
{}
//======================================================================//
//																		//
//						METHODS OF CLASS:								//
//						  PX_Box_Shape									//
//																		//
//======================================================================//
PX_Box_Shape::PX_Box_Shape( const FVec2& pos, float width, float height )
	:
	OBB { pos, width, height }
{}
 

const auto& PX_Box_Shape::Center() const
{
	return OBB.center;
}

void PX_Box_Shape::Transform( const PX_Pose_Data& pose )
{
	OBB.center += pose.pos;
	OBB.orientation = RotMtrx2( pose.orientation.rads );// *= for nice effect;
}

void PX_Box_Shape::Draw( Graphics& gfx, Color c ) const
{
	//		B ======= A
	//		=		  =
	//		=		  =
	//		C ======= D
	//
	const FVec2 length = FVec2( OBB.half_lengths );
	std::vector<FVec2> vertices =
	{
		length,
		{ -length.x, length.y },
		-length,
		{ length.x, -length.y }
	};

	

	std::for_each( vertices.begin(), vertices.end(), 
				   [&] ( FVec2& vertex ) 
				   {
					   OBB.orientation *= vertex;
					   vertex +=OBB.center;
				   } );

	//gfx.Draw_Clipped_Line( vertices [0], vertices [1], c );
	gfx.Draw_Closed_Polyline( vertices.begin(), vertices.end(), c );

	//Debug
	//FVec2 start_point = OBB.center - OBB.half_lengths * 4;
	//gfx.Draw_Line( start_point, FVec2( OBB.orientation.Basis_X() * 200 ) + start_point, Colors::Yellow );
	//gfx.Draw_Line( start_point, FVec2( OBB.orientation.Basis_Y() * 200 ) + start_point, Colors::Yellow );

	//gfx.Draw_Line( OBB.center, vertices[0], Colors::Yellow );
}

void PX_Box_Shape::Draw_test( Graphics & gfx, Color c ) const
{
	const FVec2 length = OBB.half_lengths;
	std::vector<FVec2> vertices =
	{
				length,
		{ -length.x, length.y },
			   -length,
		{ length.x, -length.y }
	};

	for ( auto& vertex : vertices )
	{
		OBB.orientation *= vertex;
		vertex += OBB.center;
	}

	

	gfx.draw_polyline_test( vertices.begin(), vertices.end(), c );

}
