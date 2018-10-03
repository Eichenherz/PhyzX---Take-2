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
#include <cassert>

Game::Game( MainWindow& wnd )
	:
	wnd( wnd ),
	gfx( wnd )
{
	particles.reserve( 3 );
	for ( size_t i = 0; i < 3; ++i )
	{
		particles.emplace_back();
		particles [i].Set_Mass( 2.0f + float( i ) );
	}

	for ( auto& p : particles )
	{
		p.Set_Vel( PX::Vec2 { 0.0f,0.0f } );
		p.Set_Damp( 0.95f );
	}

	particles[0].Set_Pos( PX::Vec2 { 420.0f,320.0f } );
	particles[1].Set_Pos( PX::Vec2 { 400.0f,280.0f } );
	particles[2].Set_Pos( PX::Vec2 { 420.0f,340.0f } );

	{
		walls.reserve( 4 );
		for ( size_t i = 0; i < 4; ++i )
		{
			walls.emplace_back();
		}

		walls [0].A = PX::Vec2 { 0.0f,0.0f };
		walls [0].B = PX::Vec2 { float( Graphics::ScreenWidth ),0.0f };
		walls [0].normal = PX::Vec2 { 0.0f,1.0f };

		walls [1].A = PX::Vec2 { float( Graphics::ScreenWidth ),0.0f };
		walls [1].B = PX::Vec2 { float( Graphics::ScreenWidth ),float( Graphics::ScreenHeight ) };
		walls [1].normal = PX::Vec2 { -1.0f,0.0f };

		walls [2].A = PX::Vec2 { float( Graphics::ScreenWidth ),float( Graphics::ScreenHeight ) };
		walls [2].B = PX::Vec2 { 0.0f,float( Graphics::ScreenHeight ) };
		walls [2].normal = PX::Vec2 { 0.0f,-1.0f };

		walls [3].A = PX::Vec2 { 0.0f,float( Graphics::ScreenHeight ) };
		walls [3].B = PX::Vec2 { 0.0f,0.0f };
		walls [3].normal = PX::Vec2 { 1.0f,0.0f };
	}
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
		const PX::Vec2 p = mouse_pos - particles[0].Get_Pos();
		particles [0].Apply_Impulse( p );
	}

	PX::Broad_Phase( particles, walls, manifolds );
	

	for ( size_t i = 0; i < 4; ++i )
	{
		PX::Filter_Contacts( manifolds );
		for ( auto& m : manifolds )
		{
			m->Solve();
		}
	}

	for ( size_t i = 0; i < 2; ++i )
	{
		// Joints
	}

	for ( auto& p : particles )
	{
		p.Update( dt );
	}
	
}

void Game::ComposeFrame()
{
	for ( auto& p : particles )
	{
		p.Debug_Draw( gfx );
	}
}
