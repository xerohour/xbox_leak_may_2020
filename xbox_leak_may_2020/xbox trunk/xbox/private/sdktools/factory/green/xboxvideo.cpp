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

#include "greenp.h"

// Constructor
CXBoxVideo::CXBoxVideo( void )
{
    XDBGTRC( APP_TITLE_NAME_A, "CXBoxVideo::CXBoxVideo()" );

    m_Device = NULL;
    m_BackBuffer = NULL;
    m_VertexBuffer = NULL;

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

