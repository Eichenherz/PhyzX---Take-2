#include "PX_Physical_Traits.h"
#include <assert.h>

//===================================================//
//													 //
//					  CONSTANTS						 //
//												     //
//===================================================//
static constexpr Scalar	kinetic_friction = 0.10f;
static constexpr Scalar	static_friction = 0.25f;
static constexpr Scalar	gravitational_const = 10.0f;
static constexpr Scalar	LINEAR_THRESHOLD = 0.1f;
static constexpr Scalar	ANGULAR_THRESHOLD = 0.5f;
//===================================================//
//													 //
//			METHODS of PX_Rigid_Body_Physics		 //
//												     //
//===================================================//
PX_Rigid_Body_Physics::PX_Rigid_Body_Physics( Scalar mass, int side, const FVec2& mass_ct, PX_Pose_Data&  pose )
	:
	mass_data					{ mass, side, mass_ct },
	pose_data					{ pose },
	kinematic_state				{ { 0.0f, 0.0f }, 0.0f },
	static_linear_friction		{ static_friction * mass_data.mass * gravitational_const },
	kinetic_linear_friction		{ kinetic_friction * mass_data.mass * gravitational_const },
	static_angular_friction		{ static_friction * mass_data.I * gravitational_const },
	kinetic_angular_friction	{ kinetic_friction * mass_data.I * gravitational_const }
{}

inline FVec2 PX_Rigid_Body_Physics::Linear_Friction() const
{
	return -resultant.force.GetNormalized() * kinetic_linear_friction;
}

inline Scalar PX_Rigid_Body_Physics::Angular_Friction() const
{
	if ( kinematic_state.angular_vel != 0.0f )
	{
		return std::copysign( kinetic_angular_friction, -kinematic_state.angular_vel );
	}
	return 0.0f;
}

void PX_Rigid_Body_Physics::Apply_Force( const FVec2& force, const FVec2& app_pt )
{
	//Apply all forces as long as they are active.
	resultant.force += force;
	resultant.torque += Perp_Dot_Prod( mass_data.center - app_pt, force );
}

void PX_Rigid_Body_Physics::Halt_Force()
{
	resultant.force = FVec2 { 0.0f,0.0f };
	resultant.torque = 0.0f;
}

void PX_Rigid_Body_Physics::Update_Kinematic_State( Scalar dt )
{
	//If moving or defeated static friction Euler integrate accl to get vel.
	if ( kinematic_state.linear_vel.GetLength() > LINEAR_THRESHOLD ||
		 resultant.force.GetLength() > static_linear_friction * static_linear_friction )
	{
		const FVec2 linear_accel = ( resultant.force + Linear_Friction() ) / mass_data.mass;
		kinematic_state.linear_vel += linear_accel * dt;
	}
	else
	{
		kinematic_state.linear_vel = FVec2 { 0.0f, 0.0f };
	}

	if ( std::fabs( kinematic_state.angular_vel ) > ANGULAR_THRESHOLD ||
		 std::fabs( resultant.torque ) > static_angular_friction )
	{
		const Scalar angular_accel = ( resultant.torque + Angular_Friction() ) / mass_data.I;
		kinematic_state.angular_vel += angular_accel * dt;
		//Kinematic_state.angular_vel *= 1.0f - dt;
	}
	else
	{
		kinematic_state.angular_vel = 0.0f;
	}
}

const PX_Kinematic_Data& PX_Rigid_Body_Physics::Kinematic_Status() const
{
	return kinematic_state;
}

PX_Kinematic_Data& PX_Rigid_Body_Physics::Kinematic_Status()
{
	return kinematic_state;
}
