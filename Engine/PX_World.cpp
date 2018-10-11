#include "PhyzX\PX_World.h"
#include "Graphics.h"

void PX::World::Init_Sym( size_t size, const std::mt19937& rng )
{
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
		walls [0].B = PX::Vec2 { float( Graphics::ScreenWidth - 4 ),4.0f };
		walls [0].normal = PX::Vec2 { 0.0f,1.0f };

		walls [1].A = PX::Vec2 { float( Graphics::ScreenWidth - 4 ),4.0f };
		walls [1].B = PX::Vec2 { float( Graphics::ScreenWidth - 4 ),float( Graphics::ScreenHeight - 4 ) };
		walls [1].normal = PX::Vec2 { -1.0f,0.0f };

		walls [2].A = PX::Vec2 { float( Graphics::ScreenWidth - 4 ),float( Graphics::ScreenHeight - 4 ) };
		walls [2].B = PX::Vec2 { 4.0f,float( Graphics::ScreenHeight - 4 ) };
		walls [2].normal = PX::Vec2 { 0.0f,-1.0f };

		walls [3].A = PX::Vec2 { 4.0f,float( Graphics::ScreenHeight - 4 ) };
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

void PX::World::Apply_Mouse_Imput( const Vec2 & pos )
{
	const PX::Vec2 p = pos - particles [0].Get_Pos();
	particles [0].Apply_Impulse( p * 10.0f );	
}

void PX::World::Update( Scalar dt )
{
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

void PX::World::Draw( Graphics & gfx )
{
	for ( auto& p : particles )
	{
		p.Debug_Draw( gfx );
	}
}
