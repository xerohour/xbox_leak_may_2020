#include "stdafx.h"
#include "polyobject.h"

// Contstructor
CPolyObject::CPolyObject()
: m_pVertexBuffer( NULL )
{
}

// Destructor
CPolyObject::~CPolyObject()
{
    // Destroy any memory, etc, that we have allocated
    CleanUp();
}


// Clean up any memory allocated, etc.
void CPolyObject::CleanUp()
{
    // Clean up our Vertex Buffer
    if( m_pVertexBuffer )
    {
        m_pVertexBuffer->Release();
        m_pVertexBuffer = NULL;
    }
}


// Initialize our data, including our vertext buffer
void CPolyObject::Initialize( IDirect3DDevice8* pD3DDevice )
{
    // Clean up anything that was previously allocated
    CleanUp();

    // Setup our vertices
    MYTEXVERTEX* pb = NULL;

    // Create our vertex buffer
    pD3DDevice->CreateVertexBuffer( sizeof( MYTEXVERTEX ) * 4, 0, 0, 0, &m_pVertexBuffer );

    m_pVertexBuffer->Lock( 0, 0, (BYTE**)&pb, 0 );

    pb[0].v.x = 0.0f;         pb[0].v.y = 0.0f;          pb[0].v.z = 0.1f; pb[0].fRHW = 0.9f; pb[0].diffuse = COLOR_WHITE; pb[0].tu = 0.0f;         pb[0].tv = 0.0f;
    pb[1].v.x = SCREEN_WIDTH; pb[1].v.y = 0.0f;          pb[1].v.z = 0.1f; pb[1].fRHW = 0.9f; pb[1].diffuse = COLOR_WHITE; pb[1].tu = SCREEN_WIDTH; pb[1].tv = 0.0f;
    pb[2].v.x = SCREEN_WIDTH; pb[2].v.y = SCREEN_HEIGHT; pb[2].v.z = 0.1f; pb[2].fRHW = 0.9f; pb[2].diffuse = COLOR_WHITE; pb[2].tu = SCREEN_WIDTH; pb[2].tv = SCREEN_HEIGHT;
    pb[3].v.x = 0.0f;         pb[3].v.y = SCREEN_HEIGHT; pb[3].v.z = 0.1f; pb[3].fRHW = 0.9f; pb[3].diffuse = COLOR_WHITE; pb[3].tu = 0.0f;         pb[3].tv = SCREEN_HEIGHT;

    m_pVertexBuffer->Unlock();
}


// Render the Poly to the screen
void CPolyObject::Render( IDirect3DDevice8* pD3DDevice )
{
    pD3DDevice->SetVertexShader( D3DFVF_MYTEXVERTEX );
    pD3DDevice->SetStreamSource( 0, m_pVertexBuffer, sizeof( MYTEXVERTEX ) );
    pD3DDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 2 );
}
