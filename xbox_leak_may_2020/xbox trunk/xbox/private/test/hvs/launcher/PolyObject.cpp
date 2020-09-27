#include "stdafx.h"
#include "polyobject.h"

// Contstructor
CPolyObject::CPolyObject( void )
: m_pVertexBuffer( NULL )
{
}

// Destructor
CPolyObject::~CPolyObject( void )
{
    // Destroy any memory, etc, that we have allocated
    CleanUp();
}


// Clean up any memory allocated, etc.
void CPolyObject::CleanUp( void )
{
    // Clean up our Vertex Buffer
    if( m_pVertexBuffer )
    {
        m_pVertexBuffer->Release();
        m_pVertexBuffer = NULL;
    }
}


// Initialize our data, including our vertext buffer
void CPolyObject::Initialize( const IDirect3DDevice8* pD3DDevice, const float fScreenWidth, const float fScreenHeight )
{
    // Clean up anything that was previously allocated
    CleanUp();

    // Setup our vertices
    MYTEXVERTEX* pb = NULL;

    // Create our vertex buffer
    pD3DDevice->CreateVertexBuffer( sizeof( MYTEXVERTEX ) * 4, 0, 0, 0, &m_pVertexBuffer );

    m_pVertexBuffer->Lock( 0, 0, (BYTE**)&pb, 0 );

    pb[0].v.x = 0.0f;         pb[0].v.y = 0.0f;          pb[0].v.z = 0.1f; pb[0].fRHW = 0.9f; pb[0].diffuse = gc_dwCOLOR_WHITE; pb[0].tu = 0.0f;         pb[0].tv = 0.0f;
    pb[1].v.x = fScreenWidth; pb[1].v.y = 0.0f;          pb[1].v.z = 0.1f; pb[1].fRHW = 0.9f; pb[1].diffuse = gc_dwCOLOR_WHITE; pb[1].tu = fScreenWidth; pb[1].tv = 0.0f;
    pb[2].v.x = fScreenWidth; pb[2].v.y = fScreenHeight; pb[2].v.z = 0.1f; pb[2].fRHW = 0.9f; pb[2].diffuse = gc_dwCOLOR_WHITE; pb[2].tu = fScreenWidth; pb[2].tv = fScreenHeight;
    pb[3].v.x = 0.0f;         pb[3].v.y = fScreenHeight; pb[3].v.z = 0.1f; pb[3].fRHW = 0.9f; pb[3].diffuse = gc_dwCOLOR_WHITE; pb[3].tu = 0.0f;         pb[3].tv = fScreenHeight;

    m_pVertexBuffer->Unlock();
}


// Render the Poly to the screen
void CPolyObject::Render( const IDirect3DDevice8* pD3DDevice )
{
    pD3DDevice->SetVertexShader( D3DFVF_MYTEXVERTEX );
    pD3DDevice->SetStreamSource( 0, m_pVertexBuffer, sizeof( MYTEXVERTEX ) );
    pD3DDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 2 );
}
