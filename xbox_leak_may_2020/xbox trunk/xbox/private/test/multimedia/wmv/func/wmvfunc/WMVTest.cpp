/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	WMVTest.cpp

Abstract:

	Created to test WMV on the xbox

Author:

	Jeff Sullivan (jeffsul) 18-Jul-2001

Revision History:

	18-Jul-2001 jeffsul
		Initial Version

--*/

#include "WMVTest.h"
#include "Ini.h"

#include <xtl.h>
#include <xdbg.h>
#include <xlog.h>
#include <macros.h>

#include <xgmath.h>

#include <stdio.h>

/*++

Routine Description:

	Test constructor

Arguments:

	None

Return Value:

	None

--*/
CWMVTest::CWMVTest()
{
	m_pDevice = NULL;
	m_nNumFilesPlayed = 0;
}


/*++

Routine Description:

	Test destructor

Arguments:

	None

Return Value:

	None

--*/
CWMVTest::~CWMVTest()
{
	RELEASE( m_pDevice );
}


/*++

Routine Description:

	read initialization variables from an ini file

Arguments:

	None

Return Value:

	S_OK on success
	E_XX on failure

--*/
HRESULT CWMVTest::ReadIni()
{
	HRESULT hr = S_OK;

	CHAR	szSearchPath[MAX_PATH];

#define GetInt( var, def )			GetIniInt( WMVTEST_APP_NAME, #var, def, WMVTEST_INI_PATH )
#define GetFloat( var, def )		GetIniFloat( WMVTEST_APP_NAME, #var, def, WMVTEST_INI_PATH )
#define GetString( var, def, ret )	GetIniString( WMVTEST_APP_NAME, #var, def, ret, strlen(ret), WMVTEST_INI_PATH )
#define GetConst( var, def )		GetIniConst( WMVTEST_APP_NAME, #var, def, WMVTEST_INI_PATH )

	GetString( WMVFileDirectory, "D:\\Media\\", m_szPath );

	sprintf( szSearchPath, "%s*.wma", m_szPath );
	
	m_hFiles = FindFirstFile( szSearchPath, &m_FindData );

	return hr;
}


/*++

Routine Description:

	initialize graphics rendering device

Arguments:

	None

Return Value:

	S_OK on success
	E_XX on failure

--*/
HRESULT CWMVTest::InitGraphics()
{
	HRESULT hr = S_OK;

	D3DPRESENT_PARAMETERS	d3dpp;

	// Set the screen mode.
    ZeroMemory(&d3dpp, sizeof(d3dpp));

    d3dpp.BackBufferWidth                 = 640;
    d3dpp.BackBufferHeight                = 480;
    d3dpp.BackBufferFormat                = D3DFMT_X8R8G8B8;
    d3dpp.BackBufferCount                 = 1;
    d3dpp.Windowed                        = FALSE;   // Must be FALSE for Xbox.
    d3dpp.EnableAutoDepthStencil          = TRUE;
    d3dpp.AutoDepthStencilFormat          = D3DFMT_D24S8;
    d3dpp.SwapEffect                      = D3DSWAPEFFECT_DISCARD;
    d3dpp.hDeviceWindow                   = NULL;
    d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	EXECUTE( Direct3D_CreateDevice(	D3DADAPTER_DEFAULT, 
									D3DDEVTYPE_HAL,
									NULL, 
									D3DCREATE_HARDWARE_VERTEXPROCESSING, 
									&d3dpp, 
									&m_pDevice) );
    if ( FAILED ( hr ) )
	{
        return hr;
    }

	// set up view and projection matrices
	XGMATRIX	matWorld, matView, matProj;

	// world matrix is simply identity
	XGMatrixIdentity( &matWorld );

	// camera is at (0,0,-1) looking at origin
    XGMatrixLookAtLH( &matView, &XGVECTOR3( 0.0f, 0.0f, -1.0f ),
                                  &XGVECTOR3( 0.0f, 0.0f, 0.0f ), 
                                  &XGVECTOR3( 0.0f, 1.0f, 0.0f ) );
	// view volume is 12x10x10, near clip plane at z=0 and far clip at z=10
	XGMatrixOrthoLH( &matProj, 12.0f, 10.0f, 1.0f, 11.0f );

	// apply the matrices to d3d
	m_pDevice->SetTransform( D3DTS_WORLD, &matWorld );
    m_pDevice->SetTransform( D3DTS_VIEW, &matView );
	m_pDevice->SetTransform( D3DTS_PROJECTION, &matProj );

	// no lighting
	m_pDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

	// no culling
	m_pDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

	return hr;
}


/*++

Routine Description:

	cleanup graphics rendering device

Arguments:

	None

Return Value:

	S_OK on success
	E_XX on failure

--*/
HRESULT CWMVTest::DestroyGraphics()
{
	HRESULT hr = S_OK;

	RELEASE( m_pDevice );

	return hr;
}


/*++

Routine Description:

	update the scene each frame

Arguments:

	None

Return Value:

	S_OK on success
	E_XX on failure

--*/
HRESULT CWMVTest::Update()
{
	HRESULT hr = S_OK;
	
	CHAR szFullName[MAX_PATH];
	
	sprintf( szFullName, "%s%s", m_szPath, m_FindData.cFileName );
	
//	EXECUTE( PlayWmvFile( szFullName, true ) );
//	if ( SUCCEEDED( hr ) )
//	{
//		m_nNumFilesPlayed++;
//	}
	
	m_bQuit = FindNextFile( m_hFiles, &m_FindData );

	return hr;
}


/*++

Routine Description:

	render the scene to the screen

Arguments:

	None

Return Value:

	S_OK on success
	E_XX on failure

--*/
HRESULT CWMVTest::Render()
{
	HRESULT hr = S_OK;
			
#if 0
	// clear the rendering surface
	EXECUTE( m_pDevice->Clear( 0, NULL, D3DCLEAR_STENCIL|D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB( 0, 0, 255 ), 1.0f, 0L ) );

	// begin the scene
	EXECUTE( m_pDevice->BeginScene() );
	if ( FAILED( hr ) )
	{
		return hr;
	}

	// submit each bit of geometry


	// render the scene to the surface
	EXECUTE( m_pDevice->EndScene() );
	if ( FAILED( hr ) )
	{
		return hr;
	}

	// flip the surface to the screen
	EXECUTE( m_pDevice->Present( NULL, NULL, NULL, NULL ) );
#endif

	return hr;
}


