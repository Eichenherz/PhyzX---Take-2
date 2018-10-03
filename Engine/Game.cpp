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
	gfx( wnd ),
	rng( rd() )
{
	const size_t size = 100;
	particles.reserve( size );
	for ( size_t i = 0; i < size; ++i )
	{
		particles.emplace_back();
	}

	for ( auto& p : particles )
	{
		p.Set_Mass( 2.0f );
		p.Set_Vel( PX::Vec2 { 0.0f,0.0f } );
		p.Set_Damp( 0.95f );


		std::uniform_int_distribution<int> x_dist( 8, 700 );
		std::uniform_int_distribution<int> y_dist( 8, 500 );
		const float x = (float) x_dist( rng );
		const float y = (float) x_dist( rng );
		p.Set_Pos( PX::Vec2 { x,y } );
	}

	/*particles[0].Set_Pos( PX::Vec2 { 420.0f,320.0f } );
	particles[1].Set_Pos( PX::Vec2 { 400.0f,280.0f } );
	particles[2].Set_Pos( PX::Vec2 { 420.0f,340.0f } );*/

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

	for ( auto& p : particles )
	{
		p.Apply_Force( PX::Vec2 { 0.0f,0.0f } );
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
