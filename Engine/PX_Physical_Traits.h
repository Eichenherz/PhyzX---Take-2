#pragma once

#include "Vec2.h"
#include "PX_Math.h"

//======================================================================//
//																		//
//						PHYSICS DATA STRUCTURES							//
//																		//
//======================================================================//
struct PX_Mass_Data // Mass_Properties
{
	FVec2	center; // of mass
	Scalar	mass;
	Scalar	I; // @ CM

	PX_Mass_Data( Scalar mass, int side, const FVec2& pos )
		:
		mass	{ mass },
		center  { pos.x + side / 2, pos.y + side / 2 },
		I		{ 1.0f/6.0f * mass * Scalar( side * side )  }
	{}
};

struct PX_Pose_Data
{
	FVec2			pos;
	Radians			orientation;

	PX_Pose_Data( const FVec2& pos, Radians dgs )
		:
		pos				{ pos },
		orientation		{ dgs }
	{}
	PX_Pose_Data( const PX_Pose_Data& p )
		: 
		pos			{ p.pos },
		orientation { p.orientation }
	{}
};

struct PX_Kinematic_Data
{
	FVec2	linear_vel;
	Scalar	angular_vel;

	PX_Kinematic_Data( const FVec2& lvel,  Scalar avel )
		:
		linear_vel		{ lvel },
		angular_vel		{ avel }
	{}
};

//======================================================================//
//																		//
//					PHYSICS RIGID BODY SIMULATION 						//
//																		//
//======================================================================//
class PX_Rigid_Body_Physics
{
public:
								PX_Rigid_Body_Physics( Scalar mass, int side, const FVec2& mass_ct, 
												   PX_Pose_Data&  pose );

	void						Apply_Force( const FVec2& force, const FVec2& app_pt );
	void						Halt_Force();
	void						Update_Kinematic_State( Scalar dt );
	const PX_Kinematic_Data&	Kinematic_Status() const;
	PX_Kinematic_Data&			Kinematic_Status();

public://private:
	PX_Mass_Data				mass_data;
	PX_Pose_Data&				pose_data;
	PX_Kinematic_Data			kinematic_state;
	struct PX_Dynamic_Data
	{
		FVec2	force = FVec2 { 0.0f,0.0f };
		Scalar	torque = Scalar(0);
	}resultant;

	const Scalar				static_linear_friction;
	const Scalar				kinetic_linear_friction;
	const Scalar				static_angular_friction;
	const Scalar				kinetic_angular_friction;

	FVec2						Linear_Friction() const;
	Scalar						Angular_Friction() const;
};