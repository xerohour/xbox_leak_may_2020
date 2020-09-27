/*****************************************************
*** xboxvideo.cpp
***
*** CPP file for our xboxvideo class.  This class
*** will initial D3D graphics, and allow the user to
*** draw text or graphics to the screen.
***
*** by James N. Helm
*** November 1st, 2000
***
*****************************************************/

#include "oobauditp.h"

// Constructor
CXBoxVideo::CXBoxVideo( void )
{
    XDBGTRC( APP_TITLE_NAME_A, "CXBoxVideo::CXBoxVideo()" );

    m_Device = NULL;
    m_BackBuffer = NULL;
    m_VertexBuffer = NULL;

    // Font properties
    m_Font = NULL;
    m_FontWidth = FONT_DEFAULT_WIDTH;
    m_FontHeight = FONT_DEFAULT_HEIGHT;
    m_FontColor = SCREEN_DEFAULT_TEXT_FG_COLOR;
    m_FontBgColor = SCREEN_DEFAULT_TEXT_BG_COLOR;

    m_ScreenWidth = SCREEN_WIDTH;       // Default the screen width to 640
    m_ScreenHeight = SCREEN_HEIGHT;     // Default the screen height to 480
}

// Destructor
CXBoxVideo::~CXBoxVideo( void )
{
    XDBGTRC( APP_TITLE_NAME_A, "CXBoxVideo::~CXBoxVideo()" );

    if( m_Device )
    {
        m_Device->Release();
        m_Device = NULL;
    }

    if( m_BackBuffer )
    {
        m_BackBuffer->Release();
        m_BackBuffer = NULL;
    }

    if( m_VertexBuffer )
    {
        m_VertexBuffer->Release();
        m_VertexBuffer = NULL;
    }
}

// Clear the screen
void CXBoxVideo::ClearScreen( DWORD color )
{
    if( FAILED( m_Device->Clear( 0, NULL, D3DCLEAR_TARGET, color, 1.0f, 0 ) ) )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CXboxVideo::ClearScreen():Failed to clear the screen!!" );
    }
}


// Draw the inputed bit array to the screen
//
// Parameters:
//      fx - the upper left x coordinate on screen
//      fy - the upper left y coordinate on screen
//      ulWidth - the width of the bitmap
//      ulHeight - the height of the bitmap
//      pData - a pointer to the array of pixel values
void CXBoxVideo::DrawBitBlt( float fx, float fy, ULONG ulWidth, ULONG ulHeight, const DWORD* pData )
{
    // Check to make sure we received a valid pointer
    if( !pData )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CXBoxVideo::DrawBitBlt():pData not a valid pointer!" );

        return;
    }

    // Draw the bitmap on the screen, pixel by pixel
	for ( ULONG y = 0; y < ulHeight; y++ )
	{
		for ( ULONG x = 0; x < ulWidth; x++ )
		{
			if ( 0 != pData[y * ulWidth + x] )
			{
				DrawLine( fx + x, fy + y, fx + x, fy + y, 1.0f, pData[y * ulWidth + x] );
			}
		}
	}
}

// Draw a box on the screen
void CXBoxVideo::DrawBox( float x1, float y1, float x2, float y2, DWORD color )
{
	MYVERTEX* pb;
	float maxx, minx;
	float maxy, miny;

	CHECK( m_Device->BeginScene() );
	CHECK( m_VertexBuffer->Lock( 0, sizeof( MYVERTEX ) * 4, (BYTE**)&pb, 0 ) );
	
	if( IsBadWritePtr( pb, 128 ) )
	{
		XDBGERR( APP_TITLE_NAME_A, "CXBoxVideo::DrawBox(): Lockreturned '%p', which is BAD!", pb );

		return;
	}

	if( x1 < x2 )
    {
		minx = x1;
		maxx = x2;
	}
    else
    {
		minx = x2;
		maxx = x1;
	}

	if( y1 < y2 )
    {
		miny = y1;
		maxy = y2;
	}
    else
    {
		miny = y2;
		maxy = y1;
	}

	if( x1 == x2 )
    {
		maxx++;
	}

	pb[0].v.x = minx; pb[0].v.y = miny; pb[0].v.z = .1f; pb[0].fRHW = .9f; pb[0].cDiffuse = color;
	pb[1].v.x = maxx; pb[1].v.y = miny; pb[1].v.z = .1f; pb[1].fRHW = .9f; pb[1].cDiffuse = color;
	pb[2].v.x = maxx; pb[2].v.y = maxy; pb[2].v.z = .1f; pb[2].fRHW = .9f; pb[2].cDiffuse = color;
	pb[3].v.x = minx; pb[3].v.y = maxy; pb[3].v.z = .1f; pb[3].fRHW = .9f; pb[3].cDiffuse = color;

	m_VertexBuffer->Unlock();

	m_Device->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);
	m_Device->EndScene();
}


// Draw a clear rectangle to the screen
void CXBoxVideo::DrawClearRect( float x1, float y1, float x2, float y2, DWORD color )
{
	D3DRECT rect;

	if ( m_Device )
	{
		if( x1 < x2 )
        {
			rect.x1 = (long)x1;
			rect.x2 = (long)x2 + 1;
		}
        else
        {
			rect.x1 = (long)x2;
			rect.x2 = (long)x1 + 1;
		}

		if( y1 < y2 )
        {
			rect.y1 = (long)y1;
			rect.y2 = (long)y2 + 1;
		}
        else
        {
			rect.y1 = (long)y2;
			rect.y2 = (long)y1 + 1;
		}

		m_Device->Clear( 1, &rect, D3DCLEAR_TARGET, color, 0, 0 );
	}
}

// Draw a line to the screen
//
// Parameters:
//      x1, y1, x2, y2 --- the coordinates. Box is drawn from (x1,y1) to (x2,y2)
//      color          --- the RGBA color to draw the box with
HRESULT CXBoxVideo::DrawLine( float x1, float y1, float x2, float y2, float fLineWidth, DWORD color )
{
    HRESULT hr = S_OK;     // Return code for D3D functions that are called, and for the function
	MYVERTEX* pb;   // Vertex information

    // Set the line width
    if( FAILED( hr = m_Device->SetRenderState( D3DRS_LINEWIDTH, *(DWORD*)&fLineWidth ) ) )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CXBoxVideo::DrawLine():Failed: m_Device->SetRenderState( D3DRS_LINEWIDTH, *(DWORD*)&fLineWidth )! - '%d'", hr );
    }

    // Before any rendering can be done, this must be called
	if( FAILED( hr = m_Device->BeginScene() ) )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CXBoxVideo::DrawLine():Problem calling m_Device->BeginScene()! - '%d'", hr );
    }

    // Locks a range of vertex data and obtains a pointer to the vertex buffer memory.
	if( FAILED( hr = m_VertexBuffer->Lock( 0, sizeof( MYVERTEX ) * NUM_VB_VERTS, (BYTE**)&pb, 0 ) ) )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CXBoxVideo::DrawLine():Problem calling m_VertexBuffer->Lock()! - '%d'", hr );
    }

    pb[0].v.x = x1;  pb[0].v.y = y1; pb[0].v.z = .1f;  pb[0].fRHW = .9f;  pb[0].cDiffuse = color;
	pb[1].v.x = x2;  pb[1].v.y = y2; pb[1].v.z = .1f;  pb[1].fRHW = .9f;  pb[1].cDiffuse = color;

    // Unlock the vertext data
	if( FAILED( hr = m_VertexBuffer->Unlock() ) )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CXBoxVideo::DrawLine():Problem calling m_VertexBuffer->Unlock()! - '%d'", hr );

        return hr;
    }

    // Renders a sequence of nonindexed, geometric primitives of the specified type from the current set of data input streams
    if( FAILED( hr = m_Device->DrawPrimitive( D3DPT_LINELIST, 0, 1 ) ) )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CXBoxVideo::DrawLine():Problem calling m_Device->DrawPrimitive()! - '%d'", hr );
    }

    // Once rendering is complete, this must be called
    // We should try this, even on a failure, in case BeginScene() was successful
    // We'll report it to the debugger, but will not return the fail/success code to the
    // caller
	if( FAILED( m_Device->EndScene() ) )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CXBoxVideo::DrawLine():Problem calling m_Device->EndScene()! - '%d'", hr );
    }

    return hr;
}

// Draw an outline to the screen
//
// Parameters:
//      x1, y1, x2, y2 --- the coordinates. Box is drawn from (x1,y1) to (x2,y2)
//      borderSize     --- width in pixels of the border
//      color          --- the RGBA color to draw the box with
void CXBoxVideo::DrawOutline( float x1, float y1, float x2, float y2, float fLineWidth, DWORD color )
{
    // Draw the box on the screen
    DrawLine( x1, y1, x1, y2, fLineWidth, color );     // Left Line
    DrawLine( x1, y2, x2, y2, fLineWidth, color );     // Bottom Line
    DrawLine( x2, y2, x2, y1, fLineWidth, color );     // Right Line
    DrawLine( x2, y1, x1, y1, fLineWidth, color );     // Top Line
}


// DeInitialize our Screen by freeing our objects
void CXBoxVideo::DeInitialize( void )
{
    if( m_BackBuffer)
    {
        m_BackBuffer->Release();
        m_BackBuffer = NULL;
    }

    if( m_Device )
    {
	    m_Device->Release();
        m_Device = NULL;
    }
}

// Prints formatted text using the default font to the screen
// Set colors to 0 to use default color
int CXBoxVideo::DrawText( float x1, float y1, DWORD fgcolor, DWORD bgcolor, const TCHAR* format, ... )
{
    if( !m_Font )
    {
        XDBGERR( APP_TITLE_NAME_A, "CXBoxVideo::DrawText():The font is not set!!" );
        return -1;
    }

	int i;                  // Return Value
	HRESULT hr;             // Return Value for XFONT
	va_list v;              // Argument List
	TCHAR wstr[256];        // Honkin Buffer
    DWORD currentFGColor;   // Current FG Color
    DWORD currentBGColor;   // Current FG Color

    // Set our FONT colors if the user wishes to
    if( fgcolor || bgcolor )
    {
        // Set the foreground color if the user wants to
        if( fgcolor )
        {
            XFONT_SetTextColor( fgcolor );
        }

        // Set the background color if the user wants to
        if( bgcolor )
        {
            XFONT_SetBkColor( bgcolor );
        }
    }

	va_start( v, format );
	i = wvsprintf( wstr, format, v );

    // Print the Text to the backbuffer
    hr = XFONT_TextOut( wstr, i, (long)x1, (long)y1 );

    // If we have a valid font object, check the HRESULT
    if( FAILED( hr ) )
    {
        XDBGERR( APP_TITLE_NAME_A, "CXBoxVideo::DrawText():XFONT_TextOut failed!!  HRESULT: %x", hr );
    }

	va_end( v );
	
    return i;
}

// Get the size of the current font
void CXBoxVideo::GetFontSize( unsigned int* pheight, unsigned int* pdecent )
{
    // Make sure we have loaded a font
    if( !m_Font )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CXBoxVideo::GetFontSize():The font is not set!!" );

        return;
    }

    // Verify the parameters that were passed in are corred
    if( ( !pheight ) || ( !pdecent ) )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CXBoxVideo::GetFontSize():Invalid arguments passed!!" );

        return;
    }

    XFONT_GetFontMetrics( pheight, pdecent );
}

// Get the pixel width of a string
//
// Parameters:
//      string -- The string to get the length in pixels
//      length -- OPTIONAL.  The length of the string (-1 if NULL terminated, this is default)
int CXBoxVideo::GetStringPixelWidth( IN LPCWSTR string, IN int length /*=-1*/ )
{
    // Make sure we have loaded a font
    if( !m_Font )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CXBoxVideo::GetStringPixelWidth( WCHAR* ):The font is not set!!" );

        return -1;
    }

    if( !string )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CXBoxVideo::GetStringPixelWidth( WCHAR* ):Invalid (string) pointer passed in!" );

        return -1;
    }

	unsigned int outlen = 0;        // Return value

	XFONT_GetTextExtent( string, length, &outlen );
	
    return outlen;
}

// Get the width of the char*
int CXBoxVideo::GetStringPixelWidth( IN char* string, IN int length /*=-1*/ )
{
    if( !string )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CXBoxVideo::GetStringPixelWidth( char* ):Invalid pointer passed in!" );

        return -1;
    }

    int len = strlen( string );

    WCHAR newWStr[MAX_PATH];
    ZeroMemory( newWStr, MAX_PATH * sizeof( WCHAR ) );

    _snwprintf( newWStr, MAX_PATH - 1, L"%S", string );

    return GetStringPixelWidth( newWStr, length );
}

// Get a D3D Transform
HRESULT CXBoxVideo::GetTransform( D3DTRANSFORMSTATETYPE State, D3DMATRIX* pMatrix )
{
    return m_Device->GetTransform( State, pMatrix );
}

// Get the D3D Vertex Shader
HRESULT CXBoxVideo::GetVertexShader( DWORD* pHandle )
{
    return m_Device->GetVertexShader( pHandle );
}

// Lighten the image specified by "pBits"
void CXBoxVideo::LightenImage( int nWidth, int nHeight, DWORD* pBits, DWORD dwAddto )
{
	DWORD dwSize = nWidth * nHeight;
	_asm{
		mov ecx, [dwSize];
		shr ecx, 1;
		movd mm0, [dwAddto];
		pshufw mm0, mm0, 01000100b
		mov esi, [pBits]
theloop:
		movq mm1, [esi]
		paddusb mm1, mm0 ;//add, saturating to 255 if wrap would occur
		movq [esi], mm1
		add esi, 8
		dec ecx
		jnz theloop

        emms
	}
}

// Darken the image specified by "pBits"
void CXBoxVideo::DarkenImage( int nWidth, int nHeight, DWORD* pBits, DWORD dwSubtract )
{
	DWORD dwSize = nWidth * nHeight;
	_asm{
		mov ecx, [dwSize];
		shr ecx, 1;
		movd mm0, [dwSubtract];
		pshufw mm0, mm0, 01000100b ;;//propegate "subtract" into both dwords of mm0
		mov esi, [pBits]
theloop:
		movq mm1, [esi]
		psubusb mm1, mm0 ;//subtract, saturating to 0 or 255 if wrap would occur
		movq [esi], mm1
		add esi, 8
		dec ecx
		jnz theloop

        emms
	}
}

// Initialize D3D for this screen
void CXBoxVideo::Initialize( int width = SCREEN_WIDTH, int height = SCREEN_HEIGHT )
{
    IDirect3D8* pD3D = NULL;
    m_ScreenWidth = width;
    m_ScreenHeight = height;

	if ( m_Device == NULL )
	{
        // Create D3D 8.
		if( FAILED( pD3D = Direct3DCreate8( D3D_SDK_VERSION ) ) )
        {
            XDBGERR( APP_TITLE_NAME_A, "CXBoxVideo::Initialize(): Failed to create pD3D!" );
			return;
        }

        // Set the screen mode.
		D3DPRESENT_PARAMETERS d3dpp;
		ZeroMemory( &d3dpp, sizeof( d3dpp ) );

		d3dpp.BackBufferWidth                 = width;
		d3dpp.BackBufferHeight                = height;
		d3dpp.BackBufferFormat                = D3DFMT_X8R8G8B8;
		d3dpp.BackBufferCount                 = 1;

        d3dpp.hDeviceWindow                   = NULL;                    // Ignored on Xbox
		d3dpp.Windowed                        = FALSE;                   // Must be false for Xbox
		d3dpp.SwapEffect                      = D3DSWAPEFFECT_DISCARD;
        d3dpp.EnableAutoDepthStencil          = FALSE; // TRUE;
        d3dpp.AutoDepthStencilFormat          = D3DFMT_D24S8;
        d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;

		// Create the m_Device.
		if ( FAILED ( pD3D->CreateDevice( D3DADAPTER_DEFAULT,
                                          D3DDEVTYPE_HAL,
                                          NULL,      // Ignored on Xbox
                                          D3DCREATE_HARDWARE_VERTEXPROCESSING,
                                          &d3dpp,
                                          &m_Device ) ) )
		{
            XDBGERR( APP_TITLE_NAME_A, "CXBoxVideo::Initialize():Failed to CreateDevice!" );
			pD3D->Release();

			return;
		}

		// Now we no longer need the D3D interface so let's free it.
		pD3D->Release();
	}

    if( FAILED( m_Device->SetRenderState( D3DRS_LIGHTING, FALSE ) ) )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CXBoxVideo::Initialize():m_Device->SetRenderState( D3DRS_LIGHTING, FALSE ) Failed!!" );
    }

    if( FAILED( m_Device->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA ) ) )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CXBoxVideo::Initialize():m_Device->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA ) Failed!!" );
    }

    if( FAILED( m_Device->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA ) ) )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CXBoxVideo::Initialize():m_Device->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA ) Failed!!" );
    }

    if( FAILED( m_Device->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE ) ) )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CXBoxVideo::Initialize():m_Device->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE ) Failed!!" );
    }

    if( FAILED( m_Device->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE ) ) )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CXBoxVideo::Initialize():m_Device->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE ) Failed!!" );
    }

    // Create our vertex buffer
	if( FAILED( m_Device->CreateVertexBuffer( sizeof( MYVERTEX ) * NUM_VB_VERTS, 0, D3DFVF_XYZRHW | D3DFVF_DIFFUSE, D3DPOOL_DEFAULT, &m_VertexBuffer ) ) )
    {
        XDBGERR( APP_TITLE_NAME_A, "CXBoxVideo::Initialize():Failed to create the Vertex Buffer!!" );
    }

    // Set the stream source
	m_Device->SetStreamSource( 0, m_VertexBuffer, sizeof( MYVERTEX ) );

    // Set our initial vertex shader that is compatible with our 2D scene
	if( FAILED( m_Device->SetVertexShader( D3DFVF_XYZRHW | D3DFVF_DIFFUSE ) ) )
    {
        XDBGERR( APP_TITLE_NAME_A, "CXBoxVideo::Initialize():m_Device->SetVertexShader( D3DFVF_XYZRHW | D3DFVF_DIFFUSE ) Failed!!" );
    }

    // Setup matricies
    // Setup our identity matrix
    D3DMATRIX mat;
    mat._11 = mat._22 = mat._33 = mat._44 = 1.0f;
    mat._12 = mat._13 = mat._14 = mat._41 = 0.0f;
    mat._21 = mat._23 = mat._24 = mat._42 = 0.0f;
    mat._31 = mat._32 = mat._34 = mat._43 = 0.0f;

    // World Matrix
    // The world matrix controls the position and orientation
    // of the polygons in world space. We'll use it later to
    // spin the triangle.
    D3DMATRIX WorldMatrix = mat;

    // Set the transforms for our 3D world
    if( FAILED( m_Device->SetTransform( D3DTS_WORLD, &WorldMatrix ) ) )
    {
        XDBGERR( APP_TITLE_NAME_A, "CXBoxVideo::Initialize():Failed to set up the World Matrix!!" );
    }

    // View Matrix
    // The view matrix defines the position and orientation of
    // the camera. Here, we are just moving it back along the z-
    // axis by 10 units.
    D3DMATRIX ViewMatrix = mat;
    ViewMatrix._43 = 10.0f;

    if( FAILED( m_Device->SetTransform( D3DTS_VIEW, &ViewMatrix ) ) )
    {
        XDBGERR( APP_TITLE_NAME_A, "CXBoxVideo::Initialize():Failed to set up the View Matrix!!" );
    }

    // Projection Matrix
    // The projection matrix defines how the 3-D scene is "projected"
    // onto the 2-D render target surface.

    // Set up a very simple projection that scales x and y
    // by 2, and translates z by -1.0.
    D3DXMATRIX ProjectionMatrix; //  = mat;
    D3DXMatrixPerspectiveFovLH( &ProjectionMatrix, 1.57f, 640.0f/480.0f, 0.001f, 30.0f );

    if( FAILED( m_Device->SetTransform( D3DTS_PROJECTION, &ProjectionMatrix ) ) )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CXBoxVideo::Initialize():Failed to set up the Projection Matrix!!" );
    }

    // Viewport
    D3DVIEWPORT8 D3DViewport;    // Direct3D Viewport

    D3DViewport.X      = 0;
    D3DViewport.Y      = 0;
    D3DViewport.Width  = width;  // rect.right - rect.left;
    D3DViewport.Height = height; // rect.bottom - rect.top;
    D3DViewport.MinZ   = 0.0f;
    D3DViewport.MaxZ   = 1.0f;

    // Set our Viewport
    if( FAILED( m_Device->SetViewport( &D3DViewport ) ) )
    {
        XDBGERR( APP_TITLE_NAME_A, "CXBoxVideo::Initialize():Failed to set the viewport!!" );
    }

    XFONT_OpenTrueTypeFont( FILE_DATA_DEFAULT_FONT_FILENAME, 4096, &m_Font );

	if( m_Font )
    {
	    if( FAILED( m_Device->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &m_BackBuffer ) ) )
        {
            XDBGERR( APP_TITLE_NAME_A, "CXBoxVideo::Initialize():m_Device->GetBackBuffer() Failed!!" );
        }

        XFONT_SelectFont( m_Font );
        XFONT_SetDrawingSurface( m_BackBuffer );
        XFONT_SetTextColor( SCREEN_DEFAULT_TEXT_FG_COLOR );
        XFONT_SetTextAlignment( XFONT_CENTER );
        XFONT_SetBkColor( SCREEN_DEFAULT_TEXT_BG_COLOR );
        XFONT_SetTextHeight( FONT_DEFAULT_HEIGHT );
        XFONT_SetTextAntialiasLevel( FONT_ALIAS_LEVEL );
        XFONT_SetTextStyle( FONT_DEFAULT_STYLE );
    }
}


// Set a D3D Transform
HRESULT CXBoxVideo::SetTransform( D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX* pMatrix )
{
    return m_Device->SetTransform( State, pMatrix );
};

// Display the current backbuffer on to the screen
void CXBoxVideo::ShowScreen()
{
	if( m_Device )
    {
		m_Device->Present( NULL, NULL, NULL, NULL );
	}
}


// Set the D3D Vertex Shader
HRESULT CXBoxVideo::SetVertexShader( DWORD Handle )
{
    return m_Device->SetVertexShader( Handle );
}


// Truncate a string to fit within a certain screen size
HRESULT CXBoxVideo::TruncateStringToFit( WCHAR* pString, unsigned int nStrSize, float x1,float x2 )
{
    if( ( !pString ) || ( 0 > ( x2 - x1 ) ) )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CXBoxVideo::TruncateStringToFit():Invalid argument!! x1 - '%f', x2 - '%f'", x1, x2 );

        return E_INVALIDARG;
    }

    float maxLen = x2 - x1;

    // If the string is to long, let's knock off characters
    while( GetStringPixelWidth( pString, -1 ) >= maxLen )
    {
        pString[wcslen( pString ) - 1] = L'\0';

        // Ensure we don't end up in an endless loop
        if( L'\0' == pString[0] )
        {
            break;
        }
    }

    return S_OK;
}


// Truncate a string to fit within a certain screen size
HRESULT CXBoxVideo::TruncateStringToFit( char* pString, unsigned int nStrSize, float x1,float x2 )
{
    if( ( !pString ) || ( 0 > ( x2 - x1 ) ) )
    {
        XDBGWRN( APP_TITLE_NAME_A, "CXBoxVideo::TruncateStringToFit():Invalid argument!! x1 - '%f', x2 - '%f'", x1, x2 );

        return E_INVALIDARG;
    }

    float maxLen = x2 - x1;

    // If the string is to long, let's knock off characters
    while( GetStringPixelWidth( pString, -1 ) >= maxLen )
    {
        pString[strlen( pString ) - 1] = '\0';

        // Ensure we don't end up in an endless loop
        if( '\0' == pString[0] )
        {
            break;
        }
    }

    return S_OK;
}
