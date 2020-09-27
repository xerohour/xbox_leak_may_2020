/*****************************************************
*** udtexture.cpp
***
*** CPP file for our udtexture class.  This class
*** will hold a texture and allow the user to perform
*** certain actions on this texture, like writing
*** text, etc
*** 
*** by James N. Helm
*** January 24th, 2001
***
*** Modified
*** 03/09/2001 - James N. Helm
*****************************************************/
#include "menuspch.h"
#include "udtexture.h"

extern XFONT *g_pFont;

// Constructor
CUDTexture::CUDTexture( void ) :
m_pD3DDevice( NULL ),
m_pTexture( NULL ),
m_pTextureSurface( NULL ),
m_pCurrentRenderTarget( NULL ),
m_pVertexBuffer( NULL ),
m_pCurrentVBuffer( NULL ),
m_uiCurrentStride( 0 ),
m_iTextureWidth( 0 ),
m_iTextureHeight( 0 ),
m_uiTextureLockedRef( 0 )
{
    XDBGTRC( MENUS_APP_TITLE_NAME_A, "CUDTexture::CUDTexture()" );
}


// Destructor
CUDTexture::~CUDTexture( void )
{
    XDBGTRC( MENUS_APP_TITLE_NAME_A, "CUDTexture::~CUDTexture()" );

    // CleanUp any memory we have allocated
    CleanUp();
}

// Clean up any memory we have allocated
void CUDTexture::CleanUp()
{
    // Release our surface if needed
    if( NULL != m_pTextureSurface )
    {
        m_pTextureSurface->Release();
        m_pTextureSurface = NULL;
    }

    // Release our Vertex Buffer
    if( m_pVertexBuffer )
    {
        m_pVertexBuffer->Release();
        m_pVertexBuffer = NULL;
    }

    // Release our texture if needed
    if( NULL != m_pTexture )
    {
        m_pTexture->Release();
        m_pTexture = NULL;
    }

    if( NULL != m_pD3DDevice )
    {
        // m_pD3DDevice->Release();
        m_pD3DDevice = NULL;
    }
}


// Clear the texture
void CUDTexture::Clear( DWORD dwColor )
{
    // Clear the texture here
    m_pD3DDevice->Clear( 0, NULL, D3DCLEAR_TARGET, dwColor, 1.0f, 0 );
}


// Draw a box on to the texture
void CUDTexture::DrawBox( float fX1, float fY1, float fX2, float fY2, DWORD dwColor )
{
    UDTEXTURE_VERTEX* pb = NULL;

    m_pVertexBuffer->Lock( 0, 0, (BYTE**)&pb, 0 );

	pb[0].v.x = fX1; pb[0].v.y = fY1; pb[0].v.z = .1f; pb[0].fRHW = .9f; pb[0].cDiffuse = dwColor;
	pb[1].v.x = fX2; pb[1].v.y = fY1; pb[1].v.z = .1f; pb[1].fRHW = .9f; pb[1].cDiffuse = dwColor;
	pb[2].v.x = fX2; pb[2].v.y = fY2; pb[2].v.z = .1f; pb[2].fRHW = .9f; pb[2].cDiffuse = dwColor;
	pb[3].v.x = fX1; pb[3].v.y = fY2; pb[3].v.z = .1f; pb[3].fRHW = .9f; pb[3].cDiffuse = dwColor;

    m_pVertexBuffer->Unlock();

    // Make sure that edge antialiasing is off for this call!
    DWORD dwEdgeCurrentState = 0;

    m_pD3DDevice->GetRenderState( D3DRS_EDGEANTIALIAS, &dwEdgeCurrentState );
    m_pD3DDevice->SetRenderState( D3DRS_EDGEANTIALIAS, FALSE );

    // Draw the box
    m_pD3DDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 2 );

    // Restore the Edge Antialiasing state
    m_pD3DDevice->SetRenderState( D3DRS_EDGEANTIALIAS, dwEdgeCurrentState );
}


// Draw a clear rectangle to the texture
void CUDTexture::DrawClearRect( float fX1, float fY1, float fX2, float fY2, DWORD dwColor )
{
	D3DRECT rect;

    rect.x1 = (long)fX1; 
	rect.x2 = (long)fX2 + 1;

    rect.y1 = (long)fY1;
	rect.y2 = (long)fY2 + 1;

    m_pD3DDevice->Clear( 1, &rect, D3DCLEAR_TARGET, dwColor, 0, 0 );
}


// Draw a line to the texture
//
// Parameters:
//      x1, y1, x2, y2 --- the coordinates. Box is drawn from (x1,y1) to (x2,y2)
//      color          --- the RGBA color to draw the box with
void CUDTexture::DrawLine( float fX1, float fY1, float fX2, float fY2, float fLineWidth, DWORD dwColor )
{
    // Set the line width
    m_pD3DDevice->SetRenderState( D3DRS_LINEWIDTH, *(DWORD*)&fLineWidth );

    UDTEXTURE_VERTEX* pb = NULL;

    m_pVertexBuffer->Lock( 0, 0, (BYTE**)&pb, 0 );

    pb[0].v.x = fX1;  pb[0].v.y = fY1; pb[0].v.z = .1f;  pb[0].fRHW = .9f;  pb[0].cDiffuse = dwColor;
	pb[1].v.x = fX2;  pb[1].v.y = fY2; pb[1].v.z = .1f;  pb[1].fRHW = .9f;  pb[1].cDiffuse = dwColor;

    m_pVertexBuffer->Unlock();

    m_pD3DDevice->DrawPrimitive( D3DPT_LINELIST, 0, 1 );
}


// Draw an outline to the texture
//
// Parameters:
//      x1, y1, x2, y2 --- the coordinates. Box is drawn from (x1,y1) to (x2,y2)
//      borderSize     --- width in pixels of the border
//      color          --- the RGBA color to draw the box with
void CUDTexture::DrawOutline( float fX1, float fY1, float fX2, float fY2, float fLineWidth, DWORD dwColor )
{
    // Draw the box on the Texture
    DrawLine( fX1, fY1, fX1, fY2, fLineWidth, dwColor );     // Left Line
    DrawLine( fX1, fY2, fX2, fY2, fLineWidth, dwColor );     // Bottom Line
    DrawLine( fX2, fY2, fX2, fY1, fLineWidth, dwColor );     // Right Line
    DrawLine( fX2, fY1, fX1, fY1, fLineWidth, dwColor );     // Top Line
}


// Prints formatted text using the default font to the texture
void CUDTexture::DrawText( float fX1, float fY1, DWORD dwFGColor, DWORD dwBGColor, const WCHAR* pwszFormat, ... )
{
	va_list ArgList;        // Argument List
    WCHAR wpszStr[256];     // Honkin Buffer

    va_start( ArgList, pwszFormat );

    XFONT_SetTextColor( g_pFont, dwFGColor );
    XFONT_SetBkColor( g_pFont, dwBGColor );

    // Create the formated string to place on to the surface
    int i = wvsprintfW( wpszStr, pwszFormat, ArgList );

    XFONT_TextOut( g_pFont, m_pTextureSurface, wpszStr, i, (long)fX1, (long)fY1 );

	va_end( ArgList );
}


// Prints formatted text using the default font to the texture
void CUDTexture::DrawText( XFONT* pXFont, float fX1, float fY1, DWORD dwFGColor, DWORD dwBGColor, const WCHAR* pwszFormat, ... )
{
    if( NULL == pXFont )
        pXFont = g_pFont;

	va_list ArgList;        // Argument List
    WCHAR wpszStr[256];     // Honkin Buffer

    va_start( ArgList, pwszFormat );

    XFONT_SetTextColor( pXFont, dwFGColor );
    XFONT_SetBkColor( pXFont, dwBGColor );

    // Create the formated string to place on to the surface
    int i = wvsprintfW( wpszStr, pwszFormat, ArgList );

    XFONT_TextOut( pXFont, m_pTextureSurface, wpszStr, i, (long)fX1, (long)fY1 );

	va_end( ArgList );
}


// Lock the texture surface so that we can render on to it
void CUDTexture::Lock()
{
    ++m_uiTextureLockedRef; // Increment our Reference count
    if( m_uiTextureLockedRef > 1 )
    {
        // Return if we've been previously locked
        return;
    }

    // Get the current render target so that we can replace it
    // after we render to our texture
    m_pD3DDevice->GetRenderTarget( &m_pCurrentRenderTarget );

    // Get the current stream source and stride so that we can replace it
    // after we finish rendering to our texture
    m_pD3DDevice->GetStreamSource( 0, &m_pCurrentVBuffer, &m_uiCurrentStride );

    if( m_pTextureSurface )
    {
        // Set our Texture surface to be our render target for all our primitive calls
        m_pD3DDevice->SetRenderTarget( m_pTextureSurface, NULL );

        // Set our stream source to be from our Vertex Buffer
        m_pD3DDevice->SetStreamSource( 0, m_pVertexBuffer, sizeof( UDTEXTURE_VERTEX ) );

        m_pD3DDevice->SetVertexShader( D3DFVF_UDTEXTURE_VERTEX );
    }
}


// Unlock the texture surface after we have rendered to it
void CUDTexture::Unlock()
{
    if( m_uiTextureLockedRef > 0 )
    {
        --m_uiTextureLockedRef; // Decrement our Reference count

        // If we are not at 0, we should return, otherwise clean up
        if( m_uiTextureLockedRef > 0 )
        {
            return;
        }
    }
    else // We must not have been locked, so simply return
    {
        return;
    }

    // Restore the original render target
    if( m_pCurrentRenderTarget )
    {
        m_pD3DDevice->SetRenderTarget( m_pCurrentRenderTarget, NULL );
        
        m_pCurrentRenderTarget->Release();
        m_pCurrentRenderTarget = NULL;
    }

    // Restore the original stream source
    if( m_pCurrentVBuffer )
    {
        m_pD3DDevice->SetStreamSource( 0, m_pCurrentVBuffer, m_uiCurrentStride );

        m_pCurrentVBuffer->Release();
        m_pCurrentVBuffer = NULL;
    }
}


// Initialize D3D for this screen
void CUDTexture::Initialize( IDirect3DDevice8* pD3DDevice, char* pszFileName=NULL, int iTextureWidth=512, int iTextureHeight=512 )
{
    XDBGTRC( MENUS_APP_TITLE_NAME_A, "CUDTexture::Initialize()" );

    // Make sure we don't leak
    CleanUp();

    // pD3DDevice->AddRef();
    m_pD3DDevice = pD3DDevice;
    m_iTextureWidth = iTextureWidth;
    m_iTextureHeight = iTextureHeight;

    /////////////////////
    // Create our texture
    /////////////////////
    if( NULL != pszFileName )
    {
        XDBGTRC( MENUS_APP_TITLE_NAME_A, "CUDTexture::Initialize():Create Texture From File..." );
        D3DXCreateTextureFromFileExA( m_pD3DDevice, 
                                      pszFileName,
                                      D3DX_DEFAULT,    // width
                                      D3DX_DEFAULT,    // height
                                      D3DX_DEFAULT,    // mips number
                                      0,
                                      D3DFMT_A8R8G8B8,
                                      D3DPOOL_MANAGED,
                                      D3DX_FILTER_LINEAR,
                                      D3DX_FILTER_LINEAR,
                                      0,
                                      NULL,
                                      NULL,
                                        &m_pTexture );
    }
    else
    {
        XDBGTRC( MENUS_APP_TITLE_NAME_A, "CUDTexture::Initialize():User Created Texture..." );
        m_pD3DDevice->CreateTexture( m_iTextureWidth,
                                     m_iTextureHeight,
                                     0,
                                     D3DUSAGE_RENDERTARGET,
                                     D3DFMT_LIN_A8R8G8B8, //D3DFMT_LIN_X8R8G8B8,
                                     0,
                                     &m_pTexture );
    }

    ///////////////////////////////
    // Get our surface from texture
    ///////////////////////////////
    XDBGTRC( MENUS_APP_TITLE_NAME_A, "CUDTexture::Initialize():Moving the Texture Surface to Video Memory..." );
    m_pTexture->MoveResourceMemory( D3DMEM_VIDEO );

    XDBGTRC( MENUS_APP_TITLE_NAME_A, "CUDTexture::Initialize():Getting the Texture Surface..." );
    m_pTexture->GetSurfaceLevel( 0, &m_pTextureSurface );


    ///////////////////////////////
    // Create a Vertex Buffer for
    // our Primitive Calls
    ///////////////////////////////
    XDBGTRC( MENUS_APP_TITLE_NAME_A, "CUDTexture::Initialize():Creating our Vertex Buffer..." );
    m_pD3DDevice->CreateVertexBuffer( sizeof( UDTEXTURE_VERTEX ) * 4, 0, 0, 0, &m_pVertexBuffer );

    XDBGTRC( MENUS_APP_TITLE_NAME_A, "CUDTexture::Initialize():Done Initialization." );
}
