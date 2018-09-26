/****************************************************************************************** 
 *	Chili DirectX Framework Version 16.07.20											  *	
 *	Game.cpp																			  *
 *	Copyright 2016 PlanetChili.net <http://www.planetchili.net>							  *
 *																						  *
 *	This file is part of The Chili DirectX Framework.									  *
 *																						  *
 *	The Chili DirectX Framework is free software: you can redistribute it and/or modify	  *
 *	it under the terms of the GNU General Public License as published by				  *
 *	the Free Software Foundation, either version 3 of the License, or					  *
 *	(at your option) any later version.													  *
 *																						  *
 *	The Chili DirectX Framework is distributed in the hope that it will be useful,		  *
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of						  *
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the						  *
 *	GNU General Public License for more details.										  *
 *																						  *
 *	You should have received a copy of the GNU General Public License					  *
 *	along with The Chili DirectX Framework.  If not, see <http://www.gnu.org/licenses/>.  *
 ******************************************************************************************/
#include "MainWindow.h"
#include "Game.h"
#include <numeric>
#include <cassert>
#include <sstream>

Game::Game( MainWindow& wnd )
	:
	wnd( wnd ),
	gfx( wnd )
{
	const auto inf = std::numeric_limits<float>::infinity();
	q0.Set_Mass( 1.0f );
	q1.Set_Mass( 2.0f );
	q2.Set_Mass( 3.0f );

	q0.Set_Pos( PX::Vec2 { 420.0f,320.0f } );
	q1.Set_Pos( PX::Vec2 { 400.0f,280.0f } );
	q2.Set_Pos( PX::Vec2 { 420.0f,340.0f } );

	q0.Set_Vel( PX::Vec2 { 0.0f,0.0f } );
	q1.Set_Vel( PX::Vec2 { 0.0f,0.0f } );
	q2.Set_Vel( PX::Vec2 { 0.0f,0.0f } );

	q0.Set_Damp( 0.95f );
	q1.Set_Damp( 0.95f );
	q2.Set_Damp( 0.95f );

	spring.freq = 3.0f;
	spring.damping_ratio = 0.1f;
	spring.rest_length = 0.01f;

	spring1.freq = 3.0f;
	spring1.damping_ratio = 0.1f;
	spring1.rest_length = 0.01f;

	spring2.freq = 3.0f;
	spring2.damping_ratio = 0.1f;

	const auto len = ( q1.Get_Pos() - q0.Get_Pos() ).GetLength();
	spring2.rest_length = 0.01f;

	spring.Init( &q0, &q1, q0.Get_Pos(), q1.Get_Pos() );
	spring1.Init( &q0, &q2, q0.Get_Pos(), q2.Get_Pos() );
	spring2.Init( &q1, &q2, q1.Get_Pos(), q2.Get_Pos() );
}

void Game::Go()
{
	gfx.BeginFrame();	
	UpdateModel();
	ComposeFrame();
	gfx.EndFrame();
}

void Game::UpdateModel()
{
	const auto dt = timer.Mark();

	const PX::Vec2 mouse_pos = wnd.mouse.GetPos();
	if ( wnd.mouse.LeftIsPressed() )
	{
		const PX::Vec2 p = mouse_pos - q0.Get_Pos();
		q0.Apply_Impulse( p );
	}

	std::wostringstream oss;

	spring.Set_Timestep( dt );
	spring1.Set_Timestep( dt );
	spring2.Set_Timestep( dt );
	for ( size_t i = 0; i < 20; ++i )
	{
		spring.Solve();
		spring1.Solve();
		spring2.Solve();

		oss << L"Pos_Err " << spring.C << L"\n";
		OutputDebugString( oss.str().c_str() );

	}
	
	q0.Update( dt );
	q1.Update( dt );
	q2.Update( dt );
}

void Game::ComposeFrame()
{
	q0.Debug_Draw( gfx );
	q1.Debug_Draw( gfx );
	q2.Debug_Draw( gfx );
}
