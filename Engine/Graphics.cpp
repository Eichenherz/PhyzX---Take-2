/******************************************************************************************
*	Chili DirectX Framework Version 16.07.20											  *
*	Graphics.cpp																		  *
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
#include "Graphics.h"
#include "DXErr.h"
#include "ChiliException.h"
#include <assert.h>
#include <string>
#include <array>
#include "Vec3.h"
#include <functional>

// Ignore the intellisense error "cannot open source file" for .shh files.
// They will be created during the build sequence before the preprocessor runs.
namespace FramebufferShaders
{
#include "FramebufferPS.shh"
#include "FramebufferVS.shh"
}

#pragma comment( lib,"d3d11.lib" )

#define CHILI_GFX_EXCEPTION( hr,note ) Graphics::Exception( hr,note,_CRT_WIDE(__FILE__),__LINE__ )

using Microsoft::WRL::ComPtr;

Graphics::Graphics( HWNDKey& key )
{
	assert( key.hWnd != nullptr );

	//////////////////////////////////////////////////////
	// create device and swap chain/get render target view
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferCount = 1;
	sd.BufferDesc.Width = Graphics::ScreenWidth;
	sd.BufferDesc.Height = Graphics::ScreenHeight;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 1;
	sd.BufferDesc.RefreshRate.Denominator = 60;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = key.hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	HRESULT				hr;
	UINT				createFlags = 0u;
#ifdef CHILI_USE_D3D_DEBUG_LAYER
#ifdef _DEBUG
	createFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
#endif
	
	// create device and front/back buffers
	if( FAILED( hr = D3D11CreateDeviceAndSwapChain( 
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		createFlags,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&sd,
		&pSwapChain,
		&pDevice,
		nullptr,
		&pImmediateContext ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating device and swap chain" );
	}

	// get handle to backbuffer
	ComPtr<ID3D11Resource> pBackBuffer;
	if( FAILED( hr = pSwapChain->GetBuffer(
		0,
		__uuidof( ID3D11Texture2D ),
		(LPVOID*)&pBackBuffer ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Getting back buffer" );
	}

	// create a view on backbuffer that we can render to
	if( FAILED( hr = pDevice->CreateRenderTargetView( 
		pBackBuffer.Get(),
		nullptr,
		&pRenderTargetView ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating render target view on backbuffer" );
	}


	// set backbuffer as the render target using created view
	pImmediateContext->OMSetRenderTargets( 1,pRenderTargetView.GetAddressOf(),nullptr );


	// set viewport dimensions
	D3D11_VIEWPORT vp;
	vp.Width = float( Graphics::ScreenWidth );
	vp.Height = float( Graphics::ScreenHeight );
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	pImmediateContext->RSSetViewports( 1,&vp );


	///////////////////////////////////////
	// create texture for cpu render target
	D3D11_TEXTURE2D_DESC sysTexDesc;
	sysTexDesc.Width = Graphics::ScreenWidth;
	sysTexDesc.Height = Graphics::ScreenHeight;
	sysTexDesc.MipLevels = 1;
	sysTexDesc.ArraySize = 1;
	sysTexDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sysTexDesc.SampleDesc.Count = 1;
	sysTexDesc.SampleDesc.Quality = 0;
	sysTexDesc.Usage = D3D11_USAGE_DYNAMIC;
	sysTexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	sysTexDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	sysTexDesc.MiscFlags = 0;
	// create the texture
	if( FAILED( hr = pDevice->CreateTexture2D( &sysTexDesc,nullptr,&pSysBufferTexture ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating sysbuffer texture" );
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = sysTexDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	// create the resource view on the texture
	if( FAILED( hr = pDevice->CreateShaderResourceView( pSysBufferTexture.Get(),
		&srvDesc,&pSysBufferTextureView ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating view on sysBuffer texture" );
	}


	////////////////////////////////////////////////
	// create pixel shader for framebuffer
	// Ignore the intellisense error "namespace has no member"
	if( FAILED( hr = pDevice->CreatePixelShader(
		FramebufferShaders::FramebufferPSBytecode,
		sizeof( FramebufferShaders::FramebufferPSBytecode ),
		nullptr,
		&pPixelShader ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating pixel shader" );
	}
	

	/////////////////////////////////////////////////
	// create vertex shader for framebuffer
	// Ignore the intellisense error "namespace has no member"
	if( FAILED( hr = pDevice->CreateVertexShader(
		FramebufferShaders::FramebufferVSBytecode,
		sizeof( FramebufferShaders::FramebufferVSBytecode ),
		nullptr,
		&pVertexShader ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating vertex shader" );
	}
	

	//////////////////////////////////////////////////////////////
	// create and fill vertex buffer with quad for rendering frame
	const FSQVertex vertices[] =
	{
		{ -1.0f,1.0f,0.5f,0.0f,0.0f },
		{ 1.0f,1.0f,0.5f,1.0f,0.0f },
		{ 1.0f,-1.0f,0.5f,1.0f,1.0f },
		{ -1.0f,1.0f,0.5f,0.0f,0.0f },
		{ 1.0f,-1.0f,0.5f,1.0f,1.0f },
		{ -1.0f,-1.0f,0.5f,0.0f,1.0f },
	};
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof( FSQVertex ) * 6;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0u;
	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = vertices;
	if( FAILED( hr = pDevice->CreateBuffer( &bd,&initData,&pVertexBuffer ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating vertex buffer" );
	}

	
	//////////////////////////////////////////
	// create input layout for fullscreen quad
	const D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 }
	};

	// Ignore the intellisense error "namespace has no member"
	if( FAILED( hr = pDevice->CreateInputLayout( ied,2,
		FramebufferShaders::FramebufferVSBytecode,
		sizeof( FramebufferShaders::FramebufferVSBytecode ),
		&pInputLayout ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating input layout" );
	}


	////////////////////////////////////////////////////
	// Create sampler state for fullscreen textured quad
	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	if( FAILED( hr = pDevice->CreateSamplerState( &sampDesc,&pSamplerState ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating sampler state" );
	}

	// allocate memory for sysbuffer (16-byte aligned for faster access)
	pSysBuffer = reinterpret_cast<Color*>( 
		_aligned_malloc( sizeof( Color ) * Graphics::ScreenWidth * Graphics::ScreenHeight,16u ) );
}

Graphics::~Graphics()
{
	// free sysbuffer memory (aligned free)
	if( pSysBuffer )
	{
		_aligned_free( pSysBuffer );
		pSysBuffer = nullptr;
	}
	// clear the state of the device context before destruction
	if( pImmediateContext ) pImmediateContext->ClearState();
}

void Graphics::EndFrame()
{
	HRESULT hr;

	// lock and map the adapter memory for copying over the sysbuffer
	if( FAILED( hr = pImmediateContext->Map( pSysBufferTexture.Get(),0u,
		D3D11_MAP_WRITE_DISCARD,0u,&mappedSysBufferTexture ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Mapping sysbuffer" );
	}
	// setup parameters for copy operation
	Color* pDst = reinterpret_cast<Color*>(mappedSysBufferTexture.pData );
	const size_t dstPitch = mappedSysBufferTexture.RowPitch / sizeof( Color );
	const size_t srcPitch = Graphics::ScreenWidth;
	const size_t rowBytes = srcPitch * sizeof( Color );
	// perform the copy line-by-line
	for( size_t y = 0u; y < Graphics::ScreenHeight; y++ )
	{
		memcpy( &pDst[ y * dstPitch ],&pSysBuffer[y * srcPitch],rowBytes );
	}
	// release the adapter memory
	pImmediateContext->Unmap( pSysBufferTexture.Get(),0u );

	// render offscreen scene texture to back buffer
	pImmediateContext->IASetInputLayout( pInputLayout.Get() );
	pImmediateContext->VSSetShader( pVertexShader.Get(),nullptr,0u );
	pImmediateContext->PSSetShader( pPixelShader.Get(),nullptr,0u );
	pImmediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	const UINT stride = sizeof( FSQVertex );
	const UINT offset = 0u;
	pImmediateContext->IASetVertexBuffers( 0u,1u,pVertexBuffer.GetAddressOf(),&stride,&offset );
	pImmediateContext->PSSetShaderResources( 0u,1u,pSysBufferTextureView.GetAddressOf() );
	pImmediateContext->PSSetSamplers( 0u,1u,pSamplerState.GetAddressOf() );
	pImmediateContext->Draw( 6u,0u );

	// flip back/front buffers
	if( FAILED( hr = pSwapChain->Present( 1u,0u ) ) )
	{
		if( hr == DXGI_ERROR_DEVICE_REMOVED )
		{
			throw CHILI_GFX_EXCEPTION( pDevice->GetDeviceRemovedReason(),L"Presenting back buffer [device removed]" );
		}
		else
		{
			throw CHILI_GFX_EXCEPTION( hr,L"Presenting back buffer" );
		}
	}
}

void Graphics::BeginFrame()
{
	// clear the sysbuffer
	//memset( pSysBuffer,0u,sizeof( Color ) * Graphics::ScreenHeight * Graphics::ScreenWidth );
	std::fill( pSysBuffer, pSysBuffer + Graphics::ScreenHeight * Graphics::ScreenWidth, Colors::Black );
}

void Graphics::PutPixel( int x,int y,Color c )
{
	assert( x >= 0 );
	assert( x < int( Graphics::ScreenWidth ) );
	assert( y >= 0 );
	assert( y < int( Graphics::ScreenHeight ) );
	pSysBuffer[Graphics::ScreenWidth * y + x] = c;
}

void Graphics::Draw_Line( FVec2 p1, FVec2 p2, Color c )
{
	const float	slope = (p2.y - p1.y )/ (p2.x - p1.x);

	if ( std::fabs( slope ) > 1.0f )
	{
		if ( p1.y > p2.y )
		{
			std::swap( p1.y, p2.y );
			std::swap( p1.x, p2.x );
		}
		const float inv_slope = 1.0f / slope;
		for ( int y = (int) p1.y, y_end = (int) p2.y; y < y_end; ++y )
		{
			const int x = int( inv_slope * ( float( y ) - p1.y ) + p1.x );
			PutPixel( x, y, c );
		}
	} 
	else 
	{
		if ( p1.x > p2.x )
		{
			std::swap( p1.x, p2.x );
			std::swap( p1.y, p2.y );
		}

		for ( int x = int( p1.x ), x_end = int( p2.x ); x < x_end; ++x )
		{
			const int y = int( slope * ( float( x ) - p1.x ) + float( p1.y ) );
			PutPixel( x, y, c );
		}

	}
}

void Graphics::Draw_Clipped_Line( FVec2 p1, FVec2 p2, Color c )
{
	constexpr float x_min = 200.0f;
	constexpr float x_max = 600.0f;
	constexpr float y_min = 100.0f;
	constexpr float y_max = 500.0f;

	std::function<unsigned char( const FVec2& )> out_code = [&] ( const FVec2& v )
	{
		// { LEFT/RIGHT/TOP/BOTTOM } bit positioning

		unsigned char code = 0;    // 0000 code for inside window

		if ( v.x < x_min )           // to the left of clip window
		{
			code = 0b1000;//code += 1 << 3;
		}
		else if ( v.x >= x_max )      // to the right of clip window
		{
			code = 0b0100;//code += 1 << 2;
		}
		if ( v.y < y_min )           // below the clip window
		{
			code |= 0b1001;//code += 1;
		}
		else if ( v.y >= y_max )      // above the clip window
		{
			code |= 0b0010;//code += 1 << 1;
		}

		return code;
	};
	
	

	const auto code1 = out_code( p1 ); 
	const auto code2 = out_code( p2 );

	// Trivial cases
	if ( (code1 | code2) == 0 )
	{
		// Trivial acception
		return Draw_Line( p1, p2, c );
	}
	if ( (code1 & code2) != 0 )
	{
		// Trivial rejection
		return;
	}

	// Non trivial cases
	// Homogeneous coordinates
	const std::array<FVec3, 4> clip_window_vertices =
	{
		FVec3 { x_min, y_min, 1.0f },
		FVec3 { x_max, y_min, 1.0f },
		FVec3 { x_max, y_max, 1.0f },
		FVec3 { x_min, y_max, 1.0f }
	};

	const std::array<FVec3, 4> clip_window_edges =
	{
		clip_window_vertices [0].cross( clip_window_vertices [1] ),
		clip_window_vertices [1].cross( clip_window_vertices [2] ),
		clip_window_vertices [2].cross( clip_window_vertices [3] ),
		clip_window_vertices [3].cross( clip_window_vertices [0] )
	};

	auto u0 = make_homogenous( p1 );
	auto u1 = make_homogenous( p2 );
	const auto line = u0.cross( u1 );

	unsigned char clip_code = 0;

	for ( size_t idx = 0; idx < 4; ++idx )
	{
		const auto projection = clip_window_vertices[idx].dot( line );
		clip_code +=	( projection >= 0 ) ? 
						( 1 << idx ) : 
						( 0 << idx );
	}
	
	if ( ( clip_code == 0x00 ) || ( clip_code == 0x0F ) )
	{
		// Excepted cases -> rejection
		return;
	}

	struct TAB_entry
	{
		unsigned char TAB1;
		unsigned char TAB2;
		unsigned char MASK;
	
		constexpr TAB_entry( unsigned char t1, unsigned char t2, unsigned char m )
			:
			TAB1 { t1 },
			TAB2 { t2 },
			MASK { m  }
		{}
	};
	constexpr std::array<TAB_entry, 15> LUT = // TAB1 , TAB2, MASK
	{
		TAB_entry { 255,255,255 },// N/A VALUE
		TAB_entry { 0,	3,	0b0100 },
		TAB_entry { 0,	1,	0b0100 },
		TAB_entry { 1,	3,	0b0010 },
		TAB_entry { 1,	2,	0b0010 },
		TAB_entry { 255, 255,255 },
		TAB_entry { 0,	2,	0b0100 },
		TAB_entry { 2,	3,	0b1000 },
		TAB_entry { 2,	3,	0b1000 },
		TAB_entry { 0,	2,	0b0100 },
		TAB_entry { 255, 255,255 },
		TAB_entry { 1,	2,	0b0010 },
		TAB_entry { 1,	3,	0b0010 },
		TAB_entry { 0,	1,	0b0100 },
		TAB_entry { 0,	3,	0b0100 }
	};

	const auto i = LUT [clip_code].TAB1;
	const auto j = LUT [clip_code].TAB2;

	if ( ( i == 255 ) || ( j == 255 ) ) return;

	if ( ( code1 != 0 ) && ( code2 != 0 ) )
	{
		u0 = line.cross( clip_window_edges [i] );
		u1 = line.cross( clip_window_edges [j] );
	}
	else if ( code1 == 0 ) // clip u1
	{
		//u1 =	( ( code2 & LUT [clip_code].MASK ) != 0 ) ?
		//		line.cross( clip_window_edges [i] ) :
		//		line.cross( clip_window_edges [j] );

		if ( ( code2 & LUT [clip_code].MASK ) != 0 )
		{
			u1 = line.cross( clip_window_edges [i] );
		}
		else
		{
			u1 = line.cross( clip_window_edges [j] );
		}
	}
	else if ( code2 == 0 ) // clip u0
	{
		//u0 =	( ( code1 & LUT [clip_code].MASK ) != 0 ) ?
		//		line.cross( clip_window_edges [i] ) :
		//		line.cross( clip_window_edges [j] );

		if ( ( code1 & LUT [clip_code].MASK ) != 0 )
		{
			u0 = line.cross( clip_window_edges [i] );
		}
		else
		{
			u0 = line.cross( clip_window_edges [j] );
		}
	}

	Draw_Line( { u0.x / u0.z, u0.y / u0.z }, 
			   { u1.x / u1.z, u1.y / u1.z }, 
			   c );
}

void Graphics::Draw_Closed_Polyline( Iter beg, Iter end, Color c )
{
	for ( auto I = beg, last = std::prev( end ); I != last; ++I )
	{
		Draw_Clipped_Line( *I, *std::next( I ), c );
	}
	Draw_Clipped_Line( *std::prev( end ), *beg, c );
}

void Graphics::draw_line_test( FVec2 p1, FVec2 p2, Color c )
{
	const float	slope = (p2.y - p1.y) / (p2.x - p1.x);
	
	if ( std::fabs( slope ) > 1.0f )
	{
		if ( p1.y > p2.y )
		{
			std::swap( p1.y, p2.y );
			std::swap( p1.x, p2.x );
		}

		const float inv_slope = 1.0f / slope;
		for ( int y = (int) (p1.y + 0.5f) , y_end = (int) p2.y; y < y_end; ++y )
		{
			const int x = int( inv_slope * ( float( y ) - p1.y ) + p1.x );
			PutPixel( x, y, c );
		}
	}
	else
	{
		if ( p1.x > p2.x )
		{
			std::swap( p1.x, p2.x );
			std::swap( p1.y, p2.y );
		}

		for ( int x = int( p1.x + 0.5f), x_end = int( p2.x ); x < x_end; ++x )
		{
			const int y = int( slope * ( float( x ) - p1.x ) + float( p1.y ) );
			PutPixel( x, y, c );
		}

	}
}

void Graphics::draw_polyline_test( Iter beg, Iter end, Color c )
{
	for ( auto I = beg, last = std::prev( end ); I != last; ++I )
	{
		draw_line_test( *I, *std::next( I ), c );
	}
	draw_line_test( *std::prev( end ), *beg, c );
}


//////////////////////////////////////////////////
//           Graphics Exception
Graphics::Exception::Exception( HRESULT hr,const std::wstring& note,const wchar_t* file,unsigned int line )
	:
	ChiliException( file,line,note ),
	hr( hr )
{}

std::wstring Graphics::Exception::GetFullMessage() const
{
	const std::wstring empty = L"";
	const std::wstring errorName = GetErrorName();
	const std::wstring errorDesc = GetErrorDescription();
	const std::wstring& note = GetNote();
	const std::wstring location = GetLocation();
	return    (!errorName.empty() ? std::wstring( L"Error: " ) + errorName + L"\n"
		: empty)
		+ (!errorDesc.empty() ? std::wstring( L"Description: " ) + errorDesc + L"\n"
			: empty)
		+ (!note.empty() ? std::wstring( L"Note: " ) + note + L"\n"
			: empty)
		+ (!location.empty() ? std::wstring( L"Location: " ) + location
			: empty);
}

std::wstring Graphics::Exception::GetErrorName() const
{
	return DXGetErrorString( hr );
}

std::wstring Graphics::Exception::GetErrorDescription() const
{
	std::array<wchar_t,512> wideDescription;
	DXGetErrorDescription( hr,wideDescription.data(),wideDescription.size() );
	return wideDescription.data();
}

std::wstring Graphics::Exception::GetExceptionType() const
{
	return L"Chili Graphics Exception";
}