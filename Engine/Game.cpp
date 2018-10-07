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
#include <algorithm>


Game::Game( MainWindow& wnd )
	:
	wnd( wnd ),
	gfx( wnd ),
	rng( rd() )
{
	const size_t size = 50;
	particles.reserve( size );
	for ( size_t i = 0; i < size; ++i )
	{
		particles.emplace_back();
	}

	for ( auto& p : particles )
	{
		p.Set_Mass( 5.0f );
		p.Set_Vel( PX::Vec2 { 0.0f,0.0f } );
		p.Set_Damp( 0.925f );
		p.Set_Restitution( 0.8f );


		std::uniform_int_distribution<int> x_dist( 100, 400 );
		std::uniform_int_distribution<int> y_dist( 100, 400 );
		const float x = (float) x_dist( rng );
		const float y = (float) x_dist( rng );
		p.Set_Pos( PX::Vec2 { x,y } );
	}



	particles [0].Set_Mass( 10.0f );
	particles [1].Set_Mass( 10.0f );
	particles [2].Set_Mass( 10.0f );

	{
		walls.reserve( 4 );
		for ( size_t i = 0; i < 4; ++i )
		{
			walls.emplace_back();
		}

		walls [0].A = PX::Vec2 { 4.0f,4.0f };
		walls [0].B = PX::Vec2 { float( Graphics::ScreenWidth-4 ),4.0f };
		walls [0].normal = PX::Vec2 { 0.0f,1.0f };

		walls [1].A = PX::Vec2 { float( Graphics::ScreenWidth-4 ),4.0f };
		walls [1].B = PX::Vec2 { float( Graphics::ScreenWidth-4 ),float( Graphics::ScreenHeight-4 ) };
		walls [1].normal = PX::Vec2 { -1.0f,0.0f };

		walls [2].A = PX::Vec2 { float( Graphics::ScreenWidth-4 ),float( Graphics::ScreenHeight -4) };
		walls [2].B = PX::Vec2 { 4.0f,float( Graphics::ScreenHeight-4 ) };
		walls [2].normal = PX::Vec2 { 0.0f,-1.0f };

		walls [3].A = PX::Vec2 { 4.0f,float( Graphics::ScreenHeight -4) };
		walls [3].B = PX::Vec2 { 4.0f,4.0f };
		walls [3].normal = PX::Vec2 { 1.0f,0.0f };
	}


	{
		s0.Init( &particles [0], &particles [1] );
		s0.rod_length = 20.f;

		s1.Init( &particles [1], &particles [2] );
		s1.rod_length = 20.f;

		s2.Init( &particles [2], &particles [0] );
		/*s2.freq = 3.0f;
		s2.damping_ratio = 0.0f;*/
		s2.rod_length = 20.0f;
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
		particles [0].Apply_Impulse( p * 10.0f );
	}

	for ( auto& p : particles )
	{
		p.Apply_Gravity( dt );
	}
	for ( auto& m : manifolds )
	{
		m->dt = dt;
	}
	PX::Broad_Phase( particles, walls, manifolds );
	PX::Filter_Contacts( manifolds );

	// Sort to resolve most significant contacts first // Thanks Ian Millington ;-) !
	std::sort( manifolds.begin(), manifolds.end(), 
			   [] ( const std::unique_ptr<PX::Manifold>& m0, const std::unique_ptr<PX::Manifold>& m1 )
			   {
				   return m1->separation > m0->separation;
			   } );

	for ( auto& m : manifolds )
	{
		m->Warm_Start();
	}
	for ( size_t i = 0; i < 4; ++i )
	{
		for ( auto& m : manifolds )
		{
			m->Solve();
		}
	}

	/*s0.Set_Timestep( dt );
	s1.Set_Timestep( dt );
	s2.Set_Timestep( dt )*/;
	for ( size_t i = 0; i < 2; ++i )
	{
		s0.Solve();
		s1.Solve();
		s2.Solve();
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
