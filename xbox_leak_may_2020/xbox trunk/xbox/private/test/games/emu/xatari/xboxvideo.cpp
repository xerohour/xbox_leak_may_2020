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

#include "xboxvideo.h"

// Constructor
CXBoxVideo::CXBoxVideo( void )
{
    DebugPrint( "CXBoxVideo::CXBoxVideo()\n" );

    m_Device = NULL;
    m_BackBuffer = NULL;
    m_VertexBuffer = NULL;
    m_State = 0L;

    // Font properties
    m_Font = NULL;
    m_FontWidth = DEFAULT_FONT_WIDTH;
    m_FontHeight = DEFAULT_FONT_HEIGHT;
    m_FontColor = DEFAULT_FONT_FG_COLOR;
    m_FontBgColor = DEFAULT_FONT_BG_COLOR;

    m_ScreenWidth = DEFAULT_SCREEN_WIDTH;       // Default the screen width to 640
    m_ScreenHeight = DEFAULT_SCREEN_HEIGHT;     // Default the screen height to 480
}

// Destructor
CXBoxVideo::~CXBoxVideo( void )
{
    DebugPrint( "CXBoxVideo::~CXBoxVideo()\n" );
}

// Clear the screen
void CXBoxVideo::ClearScreen( DWORD color )
{
    // DebugPrint( "CXBoxVideo::Clear()\n" );

    if( FAILED( m_Device->Clear( 0, NULL, D3DCLEAR_TARGET, color, 1.0f, 0 ) ) )
    {
        DebugPrint( "CXboxVideo::ClearScreen():Failed to clear the screen!!\n" );
    }

	// DrawClearRect( 0.0f, 0.0f, m_ScreenWidth - 1.0f, m_ScreenHeight - 1.0f, BACKGROUND_COLOR );
}

// Setup the proper render modes for Text Operations
void CXBoxVideo::SetD3DForText()
{
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
    // DebugPrint( "CXBoxVideo::DrawBitBlt()\n" );

    // Check to make sure we received a valid pointer
    if( !pData )
    {
        DebugPrint( "CXBoxVideo::DrawBitBlt(): pData not a valid pointer!\n" );
        return;
    }

    // Draw the bitmap on the screen, pixel by pixel
	for ( ULONG y = 0; y < ulHeight; y++ )
	{
		for ( ULONG x = 0; x < ulWidth; x++ )
		{
			if ( 0 != pData[y * ulWidth + x] )
			{
				DrawLine( fx + x, fy + y, fx + x, fy + y, 1, pData[y * ulWidth + x] );
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
		DebugPrint("CXBoxVideo::DrawBox(): Lock returned '%p', which is BAD!\n", pb);
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
    // DebugPrint( "CXBoxVideo::DrawClearRect()\n" );

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
HRESULT CXBoxVideo::DrawLine( float x1, float y1, float x2, float y2, unsigned int uiLineWidth, DWORD color )
{
    // DebugPrint( "CXBoxVideo::DrawLine()\n" );

    HRESULT hr = S_OK;     // Return code for D3D functions that are called, and for the function
	MYVERTEX* pb;   // Vertex information

    // Enable drawing of the last pixel in the target
/*    hr = m_Device->SetRenderState( D3DRS_LASTPIXEL, FALSE );
	if( D3D_OK != hr )
    {
        DebugPrint( "CXBoxVideo::DrawLine(): Problem calling m_Device->SetRenderState( FALSE )! - '%d'\n", hr );
    }
*/
    if( SUCCEEDED( hr ) )
    {
        // Before any rendering can be done, this must be called
        hr = m_Device->BeginScene();
	    if( D3D_OK != hr )
        {
            DebugPrint( "CXBoxVideo::DrawLine(): Problem calling m_Device->BeginScene()! - '%d'\n", hr );
        }
    }

    if( SUCCEEDED( hr ) )
    {
        bool bHorizontalLine = false;

        // Check to see if the line is horizontal to determine our width algorithm
        if( ( x2 - x1 ) > ( y2 - y1 ) )
        {
            bHorizontalLine = true;
        }

        for( unsigned int currPoint = 0, numLines = 0; numLines < uiLineWidth; ++numLines )
        {
            // Locks a range of vertex data and obtains a pointer to the vertex buffer memory.
            hr = m_VertexBuffer->Lock( 0, sizeof( MYVERTEX ) * NUM_VB_VERTS, (BYTE**)&pb, 0 );
	        if( D3D_OK != hr )
            {
                DebugPrint( "CXBoxVideo::DrawLine(): Problem calling m_VertexBuffer->Lock()! - '%d'\n", hr );
            }
    
            if( SUCCEEDED( hr ) )
            {
                // Verifies that the calling process has write access to the specified range of memory
	            if( IsBadWritePtr( pb, sizeof( MYVERTEX ) * NUM_VB_VERTS ) )
	            {
		            DebugPrint("CXBoxVideo::DrawLine():m_VertexBuffer->Lock() returned '%p', which is BAD!\n", pb);
                    hr = E_FAIL;
	            }
            }
            
            // Adjust our values for linewidth
            if( bHorizontalLine )
            {
                // Handle even
                if( 0 == ( numLines % 2 ) )
                {
                    y1 -= numLines;
                    y2 -= numLines;
                }
                else // Handle odd
                {
                    y1 += numLines;
                    y2 += numLines;
                }
            }
            else // Vertical, change the x
            {
                // Handle even
                if( 0 == ( numLines % 2 ) )
                {
                    x1 -= numLines;
                    x2 -= numLines;
                }
                else // Handle odd
                {
                    x1 += numLines;
                    x2 += numLines;
                }
            }

            pb[currPoint  ].v.x = x1;  pb[currPoint  ].v.y = y1; pb[currPoint  ].v.z = .1f;  pb[currPoint  ].fRHW = .9f;  pb[currPoint  ].cDiffuse = color;
	        pb[currPoint+1].v.x = x2;  pb[currPoint+1].v.y = y2; pb[currPoint+1].v.z = .1f;  pb[currPoint+1].fRHW = .9f;  pb[currPoint+1].cDiffuse = color;
                
            currPoint += 2;

            // Unlock the vertext data
	        if( FAILED( hr = m_VertexBuffer->Unlock() ) )
            {
                DebugPrint( "CXBoxVideo::DrawLine(): Problem calling m_VertexBuffer->Unlock()! - '%d'\n", hr );
                return hr;
            }
        }

        // Renders a sequence of nonindexed, geometric primitives of the specified type from the current set of data input streams
        if( FAILED( hr = m_Device->DrawPrimitive( D3DPT_LINELIST, 0, uiLineWidth ) ) )
        {
            DebugPrint( "CXBoxVideo::DrawLine(): Problem calling m_Device->DrawPrimitive()! - '%d'\n", hr );
        }
    }

    // Once rendering is complete, this must be called
    // We should try this, even on a failure, in case BeginScene() was successful
    // We'll report it to the debugger, but will not return the fail/success code to the
    // caller
	if( D3D_OK != m_Device->EndScene() )
    {
        DebugPrint( "CXBoxVideo::DrawLine(): Problem calling m_Device->EndScene()! - '%d'\n", hr );
    }
    
    // Disable drawing of the last pixel in the target
/*	if( D3D_OK != m_Device->SetRenderState( D3DRS_LASTPIXEL, TRUE ) )
    {
        DebugPrint( "CXBoxVideo::DrawLine(): Problem calling m_Device->SetRenderState( TRUE )! - '%d'\n", hr );
    }
*/
    return hr;
}

// Draw an outline to the screen
//
// Parameters:
//      x1, y1, x2, y2 --- the coordinates. Box is drawn from (x1,y1) to (x2,y2)
//      borderSize     --- width in pixels of the border
//      color          --- the RGBA color to draw the box with
void CXBoxVideo::DrawOutline( float x1, float y1, float x2, float y2, unsigned int uiLineWidth, DWORD color )
{
    for( unsigned numBoxes = 0; numBoxes < uiLineWidth; ++numBoxes )
    {
        // Adjust for line Width
        if( 0 == ( numBoxes % 2 ) )
        {
            x1 -= numBoxes;
            x2 += numBoxes;
            y1 -= numBoxes;
            y2 += numBoxes;
        }
        else
        {
            x1 += numBoxes;
            x2 -= numBoxes;
            y1 += numBoxes;
            y2 -= numBoxes;
        }

        // Draw the box on the screen
        DrawLine( x1, y1, x2, y1, 1, color );     // Top Line
        DrawLine( x2, y1, x2, y2, 1, color );     // Right Line
        DrawLine( x2, y2, x1, y2, 1, color );     // Left Line
        DrawLine( x1, y2, x1, y1, 1, color );     // Bottom Line
    }
}


// DeInitialize our Screen by freeing our objects
void CXBoxVideo::DeInitialize( void )
{
    // DebugPrint( "CXBoxVideo::DeInitialize()\n" );

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
    // DebugPrint( "CXBoxVideo::DrawText()\n" );

    if( !m_Font )
    {
        DebugPrint( "CXBoxVideo::DrawText():The font is not set!!\n" );
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
        // Get our current Font Colors
        GetFontSize( 0, 0, &currentFGColor, &currentBGColor );

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
            DebugPrint( "CXBoxVideo::DrawText():XFONT_TextOut failed!!  HRESULT: %x\n", hr );
    }

    // Set the colors back to the original if needed
    if( fgcolor || bgcolor )
    {
        // Set the foreground color back to the original if necessary
        if( fgcolor )
        {
            XFONT_SetTextColor( currentFGColor );
        }

        // Set the background color back to the original if necessary
        if( bgcolor )
        {
            XFONT_SetBkColor( currentBGColor );
        }
    }

	va_end( v );
	
    return i;
}

// Get the size of the current font
void CXBoxVideo::GetFontSize( OPTIONAL OUT float* pwidth,
                              OPTIONAL OUT float* pheight,
                              OPTIONAL OUT DWORD* pfgcolor,
                              OPTIONAL OUT DWORD* pbgcolor	)
{
    // DebugPrint( "CXBoxVideo::GetFontSize()\n" );
	
    if( pwidth )
    {
        *pwidth = m_FontWidth;
    }

	if( pheight )
    {
        *pheight = m_FontHeight;
    }

	if( pfgcolor )
    {
        *pfgcolor = m_FontColor;
    }

	if( pbgcolor )
    {
        *pbgcolor = m_FontBgColor;
    }
}

// Get the pixel width of a string
//
// Parameters:
//      string -- The string to get the length in pixels
//      length -- OPTIONAL.  The length of the string (-1 if NULL terminated, this is default)
int CXBoxVideo::GetStringPixelWidth( IN LPCWSTR string, IN int length /*=-1*/ ) 
{
    // DebugPrint( "CXBoxVideo::GetStringPixelWidth()\n" );
    if( !string )
    {
        DebugPrint( "CXBoxVideo::GetStringPixelWidth(): Invalid pointer passed in!\n" );
        return -1;
    }

    if( !m_Font )
    {
        DebugPrint( "CXBoxVideo::GetStringPixelWidth():The font is not set!!\n" );
        return -1;
    }

	unsigned int outlen = 0;        // Return value

	XFONT_GetTextExtent( string, length, &outlen );
	
    return outlen;
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

void CXBoxVideo::Initialize( IDirect3DDevice8* pDevice )
{
	DebugPrint( "CXBoxVideo::Initialize( IDirect3DDevice8* pDevice )\n" );

	if( NULL == pDevice )
	{
		DebugPrint( "CXBoxVideo::Initialize():pDevice was invalid!!\n" );
		return;
	}

	m_Device = pDevice;

    // Create our vertex buffer
    DebugPrint( "CXBoxVideo::Initialize():Creating our vertex buffer...\n" );
	if( FAILED( m_Device->CreateVertexBuffer( sizeof( MYVERTEX ) * NUM_VB_VERTS, 0, D3DFVF_XYZRHW | D3DFVF_DIFFUSE, D3DPOOL_DEFAULT, &m_VertexBuffer ) ) )
    {
        DebugPrint( "CXBoxVideo::Initialize():Failed to create the Vertex Buffer!!\n" );
    }

    // Set the stream source
	if( FAILED( m_Device->SetStreamSource( 0, m_VertexBuffer, sizeof( MYVERTEX ) ) ) )
	{
		DebugPrint( "CXBoxVideo::Initialize():m_Device->SetStreamSource() Failed!!\n" );
	}

    // Set our initial vertex shader that is compatible with our 2D scene
	if( FAILED( m_Device->SetVertexShader( D3DFVF_XYZRHW | D3DFVF_DIFFUSE ) ) )
    {
        DebugPrint( "CXBoxVideo::Initialize():m_Device->SetVertexShader( D3DFVF_XYZRHW | D3DFVF_DIFFUSE ) Failed!!\n" );
    }

    // Open Courier Font
    XFONT_OpenBitmapFont( FONT_COUR_FILENAME, 4096, &m_Font );

	if( !m_Font )
    {
        DebugPrint( "CXBoxVideo::Initialize():Failed to open font '%ws'\n", FONT_COUR_FILENAME );

        // Open Arial Font
		XFONT_OpenBitmapFont( FONT_ARIAL_FILENAME, 4096, &m_Font );

        if( !m_Font )
        {
            DebugPrint( "CXBoxVideo::Initialize():Failed to open font '%ws'\n", FONT_ARIAL_FILENAME );
            DebugPrint( "CXBoxVideo::Initialize():No font loaded!!\n" );
        }
	}

	if( m_Font )
    {
        DebugPrint( "CXBoxVideo::Initialize():Getting the back buffer...\n" );
	    if( FAILED( m_Device->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &m_BackBuffer ) ) )
        {
            DebugPrint( "CXBoxVideo::Initialize():m_Device->GetBackBuffer() Failed!!\n" );
        }

        XFONT_SelectFont( m_Font );
        XFONT_SetTextColor( DEFAULT_FONT_FG_COLOR );
        XFONT_SetBkColor( DEFAULT_FONT_BG_COLOR );
        XFONT_SetDrawingSurface( m_BackBuffer );
    }

    // Storing the state of D3D
    if( FAILED( m_Device->CreateStateBlock( D3DSBT_ALL, &m_State ) ) )
    {
        DebugPrint( "CXBoxVideo::Initialize():m_Device->CreateStateBlock( m_State ) Failed!!\n" );
    }
}

/*
// Initialize D3D for this screen
void CXBoxVideo::Initialize( int width = DEFAULT_SCREEN_WIDTH, int height = DEFAULT_SCREEN_HEIGHT )
{
    // DebugPrint( "CXBoxVideo::Initialize()\n" );

    IDirect3D8* pD3D = NULL;
    m_ScreenWidth = width;
    m_ScreenHeight = height;

	if ( m_Device == NULL )
	{
        // Create D3D 8.
        DebugPrint( "CXBoxVideo::Initialize():Creating D3D Object...\n" );
		if( FAILED( pD3D = Direct3DCreate8( D3D_SDK_VERSION ) ) )
        {
            DebugPrint( "CXBoxVideo::Initialize(): Failed to create pD3D!\n" );
			return;
        }

        // Set the screen mode.
		D3DPRESENT_PARAMETERS d3dpp;
		ZeroMemory( &d3dpp, sizeof( d3dpp ) );

		d3dpp.BackBufferWidth            = width;
		d3dpp.BackBufferHeight           = height;
		d3dpp.BackBufferFormat           = D3DFMT_X8R8G8B8;
		d3dpp.BackBufferCount            = 1;

        d3dpp.hDeviceWindow              = NULL;                    // Ignored on Xbox
		d3dpp.Windowed                   = FALSE;                   // Must be false for Xbox
		d3dpp.SwapEffect                 = D3DSWAPEFFECT_DISCARD;

		// d3dpp.EnableAutoDepthStencil     = true;
        // d3dpp.AutoDepthStencilFormat     = D3DFMT_D24S8;
		// d3dpp.FullScreen_RefreshRateInHz = 60;	//or 72?

		// Create the m_Device.
		if ( FAILED ( pD3D->CreateDevice( D3DADAPTER_DEFAULT,
                                          D3DDEVTYPE_HAL,
                                          NULL,      // Ignored on Xbox
                                          D3DCREATE_HARDWARE_VERTEXPROCESSING,
                                          &d3dpp,
                                          &m_Device ) ) )
		{
            DebugPrint( "CXBoxVideo::Initialize(): Failed to CreateDevice!\n" );
			pD3D->Release();

			return;
		}

		// Now we no longer need the D3D interface so let's free it.
		pD3D->Release();
	}

    if( FAILED( m_Device->SetRenderState( D3DRS_LIGHTING, FALSE ) ) )
    {
        DebugPrint( "CXBoxVideo::Initialize():m_Device->SetRenderState( D3DRS_LIGHTING, FALSE ) Failed!!\n" );
    }

    if( FAILED( m_Device->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA ) ) )
    {
        DebugPrint( "CXBoxVideo::Initialize():m_Device->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA ) Failed!!\n" );
    }

    if( FAILED( m_Device->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA ) ) )
    {
        DebugPrint( "CXBoxVideo::Initialize():m_Device->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA ) Failed!!\n" );
    }

    if( FAILED( m_Device->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE ) ) )
    {
        DebugPrint( "CXBoxVideo::Initialize():m_Device->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE ) Failed!!\n" );
    }

    if( FAILED( m_Device->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE ) ) )
    {
        DebugPrint( "CXBoxVideo::Initialize():m_Device->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE ) Failed!!\n" );
    }

    // Create our vertex buffer
    DebugPrint( "CXBoxVideo::Initialize():Creating our vertex buffer...\n" );
	if( FAILED( m_Device->CreateVertexBuffer( sizeof( MYVERTEX ) * NUM_VB_VERTS, 0, D3DFVF_XYZRHW | D3DFVF_DIFFUSE, D3DPOOL_DEFAULT, &m_VertexBuffer ) ) )
    {
        DebugPrint( "CXBoxVideo::Initialize():Failed to create the Vertex Buffer!!\n" );
    }

    // Set the stream source
	m_Device->SetStreamSource( 0, m_VertexBuffer, sizeof( MYVERTEX ) );

    // Set our initial vertex shader that is compatible with our 2D scene
	if( FAILED( m_Device->SetVertexShader( D3DFVF_XYZRHW | D3DFVF_DIFFUSE ) ) )
    {
        DebugPrint( "CXBoxVideo::Initialize():m_Device->SetVertexShader( D3DFVF_XYZRHW | D3DFVF_DIFFUSE ) Failed!!\n" );
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
    DebugPrint( "CXBoxVideo::Initialize():Setting the viewport...\n" );
    if( FAILED( m_Device->SetViewport( &D3DViewport ) ) )
    {
        DebugPrint( "CXBoxVideo::Initialize():Failed to set the viewport!!\n" );
    }
    
    // Open Courier Font
    XFONT_OpenBitmapFont( FONT_COUR_FILENAME, 4096, &m_Font );

	if( !m_Font )
    {
        DebugPrint( "CXBoxVideo::Initialize(): Failed to open font '%ws'\n", FONT_COUR_FILENAME );

        // Open Arial Font
		XFONT_OpenBitmapFont( FONT_ARIAL_FILENAME, 4096, &m_Font );

        if( !m_Font )
        {
            DebugPrint( "CXBoxVideo::Initialize(): Failed to open font '%ws'\n", FONT_ARIAL_FILENAME );
            DebugPrint( "CXBoxVideo::Initialize(): No font loaded!!\n" );
        }
	}

	if( m_Font )
    {
        DebugPrint( "CXBoxVideo::Initialize():Getting the back buffer...\n" );
	    if( FAILED( m_Device->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &m_BackBuffer ) ) )
        {
            DebugPrint( "CXBoxVideo::Initialize():m_Device->GetBackBuffer() Failed!!\n" );
        }

        XFONT_SelectFont( m_Font );
        XFONT_SetTextColor( DEFAULT_FONT_FG_COLOR );
        XFONT_SetBkColor( DEFAULT_FONT_BG_COLOR );
        XFONT_SetDrawingSurface( m_BackBuffer );
    }
}
*/

// Set the font dimensions
void CXBoxVideo::SetFontSize( float width, float height, DWORD color, DWORD bkcolor )
{
    // DebugPrint( "CXBoxVideo::SetFontSize()\n" );

    if( !m_Font )
    {
        DebugPrint( "CXBoxVideo::SetFontSize():The font is not set!!\n" );
        return;
    }

	m_FontWidth     = width;
    m_FontHeight    = height; 
	m_FontColor     = color;
	m_FontBgColor   = bkcolor;

    XFONT_SetTextColor( m_FontColor );
    XFONT_SetBkColor( m_FontBgColor );
}

// Set a D3D Transform
HRESULT CXBoxVideo::SetTransform( D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX* pMatrix )
{
    return m_Device->SetTransform( State, pMatrix );
};

// Display the current backbuffer on to the screen
void CXBoxVideo::ShowScreen()
{
    // DebugPrint( "CXBoxVideo::ShowScreen()\n" );

#ifdef _UseDxConio_
    xDisplayConsole();
#endif

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