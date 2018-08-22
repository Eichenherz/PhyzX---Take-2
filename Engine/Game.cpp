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
	pose( { 400.0f,300.0f }, angle ),
	box( pose.pos, 100.0f, 100.0f ),
	clip_window ( pose.pos, 400.0f, 400.0f )
{
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
	if ( wnd.mouse.LeftIsPressed()  )
	{
		angle += Radians { 0.10f };
	}
	if ( wnd.mouse.RightIsPressed() )
	{
		angle += Radians { -0.10f };
	}

	FVec2 delta_pos { 0,0 };
	if ( wnd.kbd.KeyIsPressed( VK_UP ) )
	{
		delta_pos += { 0, -1 };
	}
	else if ( wnd.kbd.KeyIsPressed( VK_DOWN ) )
	{
		delta_pos += { 0, 1 };
	}
	else if ( wnd.kbd.KeyIsPressed( VK_LEFT ) )
	{
		delta_pos += { -1, 0 };
	}
	else if ( wnd.kbd.KeyIsPressed( VK_RIGHT ) )
	{
		delta_pos += { 1, 0 };
	}

	pos = delta_pos;

	box.Transform( PX_Pose_Data { pos,angle } );
	//mouse_pos = wnd.mouse.GetPos();
}

void Game::ComposeFrame()
{
	clip_window.Draw_test( gfx, Colors::Red );
	//box.Draw_test( gfx, Colors::Blue );
	//box.Draw( gfx, Colors::White );
	
	IVec2 mouse_pos = { 700,300 };
	gfx.draw_line_test( FVec2(mouse_pos), { 400.0f,300.0f }, Colors::Blue );
	gfx.Draw_Clipped_Line( FVec2(mouse_pos), { 400.0f,300.0f }, Colors::Green );
}
