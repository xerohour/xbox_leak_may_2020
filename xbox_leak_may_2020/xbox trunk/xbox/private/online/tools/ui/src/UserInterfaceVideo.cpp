///////////////////////////////////////////////////////////////////////////////
//
// Name: UserInterfaceMenu.cpp
//
// Author: Dan Rosenstein (danrose)
//
// Description: the implementation for the base menu class
//
///////////////////////////////////////////////////////////////////////////////

#include "UserInterfaceVideo.h"
#include "UserInterfaceScreen.h"
#include "UserInterfaceMenu.h"

///////////////////////////////////////////////////////////////////////////////
//
// Name: CUserInterfaceVideo
//
// Input: None
//
// Output: an instance of the user interface video class
//
// Description: constructs the video for the UI
//
///////////////////////////////////////////////////////////////////////////////

CUserInterfaceVideo::CUserInterfaceVideo( void )
	: m_pDevice( NULL ),
	  m_pScreen( NULL ),
	  m_pFont( NULL ),
      m_pBackBuffer( NULL ),
	  m_uHeight( 0 ),
	  m_uWidth( 0 ),
	  m_dwClearColor( 0 ),
	  m_pBitmap( NULL )
{
	//
	// no code here
	//
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: ~CUserInterfaceVideo
//
// Input: None
//
// Output: None
//
// Description: destructs the video ui
//
///////////////////////////////////////////////////////////////////////////////

CUserInterfaceVideo::~CUserInterfaceVideo( void )
{
	//
	// delete font
	//

	if ( m_pFont )
	{
		XFONT_Release(m_pFont);
		m_pFont = NULL;
	}

    //
    // release the back buffer
    //

    if ( m_pBackBuffer )
    {
        m_pBackBuffer->Release();
    }

	//
	// delete the bitmap
	//

	if ( m_pBitmap )
	{
		delete m_pBitmap;
		m_pBitmap = NULL;
	}

	//
	// shut down graphics
	//

	if ( m_pDevice )
	{
		m_pDevice->Release();
		m_pDevice = NULL;
	}
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: Initialize
//
// Input: pScreen - the root screen
//        uWidth - the width of the d3d screen
//        uHeight - the height of the d3d screen
//        dwClearColor - the color to clear with
//        szFileName - the name of the bitmap file
//
// Output: S_OK if input creation was successful, E_XXXX otherwise
//
// Description: sets up the app for displaying video
//
///////////////////////////////////////////////////////////////////////////////

HRESULT
CUserInterfaceVideo::Initialize( CUserInterfaceScreen* pScreen,
								 UINT uWidth, 
								 UINT uHeight,
								 DWORD dwClearColor, 
								 LPSTR szFileName )
{
	HRESULT hr = S_OK;
	PDIRECT3D8 pD3D = NULL;
	D3DSurface* pBackBuffer = NULL;

	D3DPRESENT_PARAMETERS d3dpp;

	//
	// insert screen
	//

	m_pScreen = pScreen;

	//
	// width, height
	//

	m_uWidth = uWidth;
	m_uHeight = uHeight;

	//
	// clear color
	//

	m_dwClearColor = dwClearColor;

	//
	// zero out the presentation params
	//

	ZeroMemory( &d3dpp, sizeof( D3DPRESENT_PARAMETERS ) );

	//
	// Create d3d
	//

	if ( SUCCEEDED( hr ) )
	{
		pD3D = Direct3DCreate8( D3D_SDK_VERSION );
		hr = pD3D ? S_OK : E_OUTOFMEMORY;

		ASSERT( SUCCEEDED( hr ) || ! "Could not create d3d object" );
	}

	if ( SUCCEEDED( hr ) )
	{	
		
		//
        // Set the screen mode.
		//

		d3dpp.BackBufferWidth                 = uWidth;
		d3dpp.BackBufferHeight                = uHeight;
		d3dpp.BackBufferFormat                = D3DFMT_X8R8G8B8;
		d3dpp.BackBufferCount                 = 1;
        d3dpp.hDeviceWindow                   = NULL;               
		d3dpp.Windowed                        = FALSE;                 
		d3dpp.SwapEffect                      = D3DSWAPEFFECT_DISCARD;
        d3dpp.EnableAutoDepthStencil          = FALSE; 
        d3dpp.AutoDepthStencilFormat          = D3DFMT_D24S8;
        d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
		d3dpp.FullScreen_RefreshRateInHz      = D3DPRESENT_RATE_DEFAULT;
		d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_ONE;

		//
		// Create the device
		//

		hr = pD3D->CreateDevice( D3DADAPTER_DEFAULT,
                                 D3DDEVTYPE_HAL,
                                 NULL,
                                 D3DCREATE_HARDWARE_VERTEXPROCESSING,
                                 &d3dpp,
                                 &m_pDevice );
	
		ASSERT( SUCCEEDED( hr ) || !"could not create d3d device" );
	}

	//
	// open the true type font
	//

    if ( SUCCEEDED( hr ) )
	{
		hr = XFONT_OpenTrueTypeFont( L"d:\\media\\fonts\\tahoma.ttf", 4096, &m_pFont );
		ASSERT( SUCCEEDED( hr ) || !"Could not open font" );
	}

	//
	// get the back buffer
	//

	if( SUCCEEDED( hr ) )
    {
	    hr = m_pDevice->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &m_pBackBuffer );
		ASSERT( SUCCEEDED( hr ) || !"could not get back buffer" );
	}

	//
	// set font properties
	//

	if ( SUCCEEDED( hr ) )
	{
        XFONT_SetTextColor( m_pFont, 0 );
        XFONT_SetTextAlignment( m_pFont, XFONT_CENTER );
        XFONT_SetBkColor( m_pFont, 0 );
        XFONT_SetTextHeight( m_pFont, 20 );
        XFONT_SetTextAntialiasLevel( m_pFont, 0 );
        XFONT_SetTextStyle( m_pFont, XFONT_NORMAL );
		XFONT_SetBkMode( m_pFont, XFONT_OPAQUE );
    }

	//
	// load the bitmap
	//

	if ( SUCCEEDED( hr ) && szFileName )
	{
		m_pBitmap = new CBitmapFile();
		hr = m_pBitmap ? S_OK : E_OUTOFMEMORY;

		ASSERT( SUCCEEDED( hr ) || !"could not create bitmap" );

		if ( SUCCEEDED( hr ) )
		{
			hr = m_pBitmap->Read( 0, 0, szFileName );
			ASSERT( SUCCEEDED( hr ) || !"Could not read from file" );
		}
	}


	//
	// Now we no longer need the D3D interface so let's free it.
	//

	if ( pD3D )
	{
		pD3D->Release();
		pD3D = NULL;
	}

	//
	// if there was a failure, we don't want the bitmap
	//

	if ( FAILED( hr ) && m_pBitmap )
	{
		delete m_pBitmap;
		m_pBitmap = NULL;
	}

	return hr;
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: Update
//
// Input: pInput - the input state
//        pAudio - the audio class
//
// Output: S_OK on success. E_XXXX on failure
//
// Description: Renders the User interface 
//
///////////////////////////////////////////////////////////////////////////////

HRESULT
CUserInterfaceVideo::Update( CUserInterfaceInput* pInput, CUserInterfaceAudio* pAudio )
{
	HRESULT hr = S_OK;
	CUserInterfaceScreen* pNextScreen = NULL;

	//
	// check to make sure the device is allocated
	//

	ASSERT( m_pDevice || !"d3d device not allocated" );

	if ( ! m_pDevice )
	{
		hr = E_POINTER;
	}

	//
	// check to make sure screen exists
	//

	ASSERT( m_pScreen || !"screen needs to be inserted before updating" );

	if ( ! m_pScreen )
	{
		hr = E_POINTER;
	}

	//
	// clear the screen
	//

	if ( SUCCEEDED( hr ) )
	{
		hr = ClearScreen( m_dwClearColor );
		ASSERT( SUCCEEDED( hr ) || ! "Could not clear screen" );
	}

	//
	// update the screen
	//

	if ( SUCCEEDED( hr ) )
	{
		pNextScreen = m_pScreen->Update( pInput, pAudio );

		//
		// we may want to stay on this screen
		//

		if ( pNextScreen )
		{
			m_pScreen = pNextScreen;
		}
	}

	//
	// draw the current screen to the graphics device
	//

	if ( SUCCEEDED( hr ) )
	{
		hr = DrawScreen();
		ASSERT( SUCCEEDED( hr ) || ! "Could not draw screen" );
	}

	//
	// render the screen
	//

	if ( SUCCEEDED( hr ) )
	{
		hr = m_pDevice->Present( NULL, NULL, NULL, NULL );
		ASSERT( SUCCEEDED( hr ) || !"could not present scene" );
	}

	//
	// set soft rendering
	//

	static BOOL bPresented = FALSE;

	if (! bPresented && SUCCEEDED( hr ) )
	{
		m_pDevice->SetSoftDisplayFilter( TRUE );
		bPresented = TRUE;
	}

	return hr;
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: ClearScreen
//
// Input: dwColor - 32 bit ARGB color
//
// Output: S_OK on success. E_XXXX on failure
//
// Description: clears the screen on the device
//
///////////////////////////////////////////////////////////////////////////////

HRESULT
CUserInterfaceVideo::ClearScreen( DWORD dwColor )
{
	//
	// make sure device was allocated, clear the screen
	//

	ASSERT( m_pDevice || !"d3d device not allocated" );

	if ( m_pDevice )
	{
		return m_pDevice->Clear( 0, NULL, D3DCLEAR_TARGET, dwColor, 0.0f, 0 );
	}
	else
	{
		return E_POINTER;
	}
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: DrawScreen
//
// Input: None
//
// Output: S_OK on success. E_XXXX on failure
//
// Description: draws the current screen 
//
///////////////////////////////////////////////////////////////////////////////

HRESULT
CUserInterfaceVideo::DrawScreen( void )
{
	HRESULT hr = S_OK;
	CUserInterfaceMenu* pMenu = NULL;
	CUserInterfaceText* pText = NULL;
	LPSTR szItem = NULL;

	//
	// make sure device was allocated
	//

	ASSERT( m_pDevice || !"d3d device not allocated" );
	
	if ( ! m_pDevice )
	{
		hr = E_POINTER;
	}

	//
	// make sure there is a screen
	//

	if ( SUCCEEDED( hr ) )
	{
		hr = m_pScreen ? S_OK : E_FAIL;
		ASSERT( SUCCEEDED( hr ) || !"need to insert a screen before drawing!" );
	}

	//
	// draw the background
	//

	if ( SUCCEEDED( hr ) && m_pBitmap )
	{
		hr = m_pBitmap->Render( m_pDevice );
		ASSERT( SUCCEEDED( hr ) || !" could not render background" );
	}

	//
	// deal with a screen that has text
	//

	if ( SUCCEEDED( hr ) && m_pScreen->GetText() )
	{
		hr = DrawText( m_pScreen->TextX(), 
					   m_pScreen->TextY(), 
					   m_pScreen->FGColor(),
					   m_pScreen->BGColor(),
					   L"%S", 
					   m_pScreen->GetText() );

		ASSERT( SUCCEEDED( hr ) || !"Could not write text" );
	}

	//
	// the screen has no text. It should have menus
	//

	else
	{

	//
	// loop through the menus
	//

		for ( DWORD dwMenu = 0; 
			  SUCCEEDED( hr ) && dwMenu < m_pScreen->NumMenus(); 
			  dwMenu++ )
		{
			//
			// find hte active menu
			//

			if ( m_pScreen->ActiveMenu( dwMenu ) )
				break;
		}

		if ( SUCCEEDED( hr ) )
		{
			//
			// save the menu
			//

			pMenu = m_pScreen->Menu( dwMenu );
			ASSERT( pMenu || ! "menu was NULL!" );

			if ( ! pMenu )
			{
				hr = E_POINTER;
			}

			//
			// draw the title if it exists
			//

			if ( SUCCEEDED( hr ) && 0 != pMenu->GetTitle() )
			{
				hr = DrawText( pMenu->TextX(), 
							   pMenu->TextY() - 28, 
							   pMenu->FGColor(),
							   pMenu->BGColor(),
							   L"%S", 
							   pMenu->GetTitle() );

				ASSERT( SUCCEEDED( hr ) || !"Could not render empty list" );
			}

			//
			// deal with an empty menu
			//

			if ( SUCCEEDED( hr ) && 0 == pMenu->NumItems() )
			{
				hr = DrawText( pMenu->TextX(), 
							   pMenu->TextY(), 
							   m_pScreen->ActiveMenu( dwMenu ) ? 
							   pMenu->BGColor() : 
							   pMenu->FGColor(),
							   m_pScreen->ActiveMenu( dwMenu ) ? 
							   pMenu->FGColor() : 
							   pMenu->BGColor(),
							   L"%S", 
							   "Empty" );

				ASSERT( SUCCEEDED( hr ) || !"Could not render empty list" );
			}

			//
			// loop through the items in the menu
			//

			for ( DWORD dwItem = 0; 
			      dwItem < pMenu->NumItems() && SUCCEEDED( hr ); 
				  dwItem++ )
			{
				//
				// get the item in the menu
				//

				szItem = pMenu->Item( dwItem );

				//
				// draw and highlite it if it is active
				//

				hr = DrawText( pMenu->TextX(), 
							   pMenu->TextY() + dwItem * 28,
							   pMenu->ActiveItem( dwItem ) && 
							   m_pScreen->ActiveMenu( dwMenu ) ? 
							   pMenu->BGColor( dwItem ) : 
  							   pMenu->FGColor( dwItem ),
							   pMenu->ActiveItem( dwItem ) &&
							   m_pScreen->ActiveMenu( dwMenu ) ? 
							   pMenu->FGColor( dwItem ) : 
  							   pMenu->BGColor(dwItem ),
							   L"%S", 
							   szItem );

				ASSERT( SUCCEEDED( hr ) || !"Could not draw text" );

			}

		}
	}

	//
	// draw the rest of the UI text
	//

	for ( DWORD i = 0; SUCCEEDED( hr ) && i < m_pScreen->NumUIText(); i++ )
	{
		//
		// get each text object
		//

		pText = m_pScreen->UIText( i );

		//
		// draw it to the screen, if it is not hidden
		//

		if ( ! pText->Hidden() )
		{
			hr = DrawText( pText->X(),
						   pText->Y(),
						   pText->FGColor(),
						   pText->BGColor(),
						   L"%S",
						   pText->Text() );

			ASSERT( SUCCEEDED( hr ) || !"Could not draw ui text" );
		}
	}

	return hr;
}

///////////////////////////////////////////////////////////////////////////////
//
// Name: DrawText
//
// Input: lX - the x location
//        lY - the y location
//        dwFGColor - the fg color
//        dwBGColor - the bg color
//        wszFormat - printf compatible format
//
// Output: S_OK if drawing was successful, E_XXX otherwise
//
// Description: prints text to the screen
//
///////////////////////////////////////////////////////////////////////////////

HRESULT
CUserInterfaceVideo::DrawText( LONG lX, 
							   LONG lY, 
							   DWORD dwFGColor, 
							   DWORD dwBGColor, 
							   LPCWSTR wszFormat, 
							   ... )
{
	HRESULT hr = S_OK;
	int i = 0;
	va_list v;
	WCHAR wszStr[256];

	//
	// Set the foreground color
	//

	XFONT_SetTextColor( m_pFont, dwFGColor );

	//
	// Set the background color
	//

	XFONT_SetBkColor( m_pFont, dwBGColor );

	//
	// print the formatted string to the buffer
	//

	va_start( v, wszFormat );
	i = wvsprintfW( wszStr, wszFormat, v );
	va_end( v );

	//
    // Print the Text to the backbuffer
	//

	hr = XFONT_TextOut( m_pFont, m_pBackBuffer, wszStr, i, lX, lY );
	ASSERT( SUCCEEDED( hr ) || !"Could not set output text" );
	
    return hr;
}


