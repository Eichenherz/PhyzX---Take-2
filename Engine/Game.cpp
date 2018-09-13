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

Game::Game( MainWindow& wnd )
	:
	wnd( wnd ),
	gfx( wnd )
{
	q0.Set_Mass( 2.0f );
	q1.Set_Mass( 2.0f );

	q0.Set_Pos( PX::Vec2 { 380.0f,300.0f } );
	q1.Set_Pos( PX::Vec2 { 420.0f,300.0f } );

	q0.Set_Vel( PX::Vec2 { 0.0f,0.0f } );
	q1.Set_Vel( PX::Vec2 { 0.0f,0.0f } );

	q0.Set_Damp( 0.95f );
	q1.Set_Damp( 0.95f );

	rod.Init( &q0, &q1, q0.Get_Pos(), q1.Get_Pos() );
	rod.rod_length = 40.0f;
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

	rod.SolveVel();
	q0.Update( dt );
	q1.Update( dt );
}

void Game::ComposeFrame()
{
	q0.Debug_Draw( gfx );
	q1.Debug_Draw( gfx );
}
