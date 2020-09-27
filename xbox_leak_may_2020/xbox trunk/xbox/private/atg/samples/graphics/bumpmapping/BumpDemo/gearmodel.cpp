//-----------------------------------------------------------------------------
// File: GearModel.cpp
//
// Desc: Model of a gear
//
// Hist: 02.19.00 - New for March XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <xtl.h>
#include <XBUtil.h>
#include "GearModel.h"




//-----------------------------------------------------------------------------
// Custom vertex type
//-----------------------------------------------------------------------------
struct D3DVERTEX
{
    D3DXVECTOR3 pos;
    D3DXVECTOR3 normal;
    FLOAT       tu, tv;
};




//-----------------------------------------------------------------------------
// Basis vectors for a vertex
//-----------------------------------------------------------------------------
struct VERTEXBASIS
{
    D3DXVECTOR3 S;
    D3DXVECTOR3 T;
    D3DXVECTOR3 SxT;
};




inline VOID FILL_VERTEX( D3DVERTEX* v, FLOAT x, FLOAT y, FLOAT z, 
                         FLOAT nx, FLOAT ny, FLOAT nz, FLOAT tu, FLOAT tv )
{
    v->pos    = D3DXVECTOR3( x, y, z );
    v->normal = D3DXVECTOR3( nx, ny, nz );
    v->tu     = tu;
    v->tv     = tv;
}




inline VOID FILL_VERTEX( D3DVERTEX* v, D3DXVECTOR3& p, D3DXVECTOR3& n, 
                         FLOAT tu, FLOAT tv )
{
    v->pos    = p;
    v->normal = n;
    v->tu     = tu;
    v->tv     = tv;
}




//-----------------------------------------------------------------------------
// Name: CreateBasisVectors()
// Desc: Creates basis vectors for a triangle
//-----------------------------------------------------------------------------
VOID CreateBasisVectors( D3DVERTEX& v0, D3DVERTEX& v1, D3DVERTEX& v2, 
                         VERTEXBASIS& b0, VERTEXBASIS& b1, VERTEXBASIS& b2 )
{
    static const float SMALL_FLOAT = 1e-12f;

    float ds1 = v1.tu - v0.tu;
    float dt1 = v1.tv - v0.tv;

    float ds2 = v2.tu - v0.tu;
    float dt2 = v2.tv - v0.tv;

    D3DXVECTOR3 edge01;
    D3DXVECTOR3 edge02;
    D3DXVECTOR3 cp;

    // x, s, t
    edge01 = D3DXVECTOR3( v1.pos.x - v0.pos.x, ds1, dt1 );
    edge02 = D3DXVECTOR3( v2.pos.x - v0.pos.x, ds2, dt2 );

    D3DXVec3Cross(&cp, &edge01, &edge02);
    if ( fabs(cp.x) > SMALL_FLOAT )
    {
        float dsdx = -cp.y / cp.x;
        float dtdx = -cp.z / cp.x;

        b0.S.x += dsdx;
        b0.T.x += dtdx;

        b1.S.x += dsdx;
        b1.T.x += dtdx;

        b2.S.x += dsdx;
        b2.T.x += dtdx;
    }

    // y, s, t
    edge01 = D3DXVECTOR3( v1.pos.y - v0.pos.y, ds1, dt1 );
    edge02 = D3DXVECTOR3( v2.pos.y - v0.pos.y, ds2, dt2 );

    D3DXVec3Cross(&cp, &edge01, &edge02);
    if ( fabs(cp.x) > SMALL_FLOAT )
    {
        float dsdx = -cp.y / cp.x;
        float dtdx = -cp.z / cp.x;

        b0.S.y += dsdx;
        b0.T.y += dtdx;

        b1.S.y += dsdx;
        b1.T.y += dtdx;

        b2.S.y += dsdx;
        b2.T.y += dtdx;
    }

    // z, s, t
    edge01 = D3DXVECTOR3( v1.pos.z - v0.pos.z, ds1, dt1 );
    edge02 = D3DXVECTOR3( v2.pos.z - v0.pos.z, ds2, dt2 );

    D3DXVec3Cross(&cp, &edge01, &edge02);
    if ( fabs(cp.x) > SMALL_FLOAT )
    {
        float dsdx = -cp.y / cp.x;
        float dtdx = -cp.z / cp.x;

        b0.S.z += dsdx;
        b0.T.z += dtdx;

        b1.S.z += dsdx;
        b1.T.z += dtdx;

        b2.S.z += dsdx;
        b2.T.z += dtdx;
    }
}




//-----------------------------------------------------------------------------
// Name: CreateBasisVectorsVB()
// Desc: Creates basis vectors for a mesh
//-----------------------------------------------------------------------------
LPDIRECT3DVERTEXBUFFER8 CreateBasisVectorsVB( LPDIRECT3DDEVICE8 pd3dDevice,
                                              D3DPRIMITIVETYPE dwPrimType, 
                                              D3DVERTEX* pVertices, DWORD dwNumVertices,
                                              WORD* pIndices, DWORD dwNumIndices )
{
    // Create a vertex buffer
    LPDIRECT3DVERTEXBUFFER8 pBasisVB;
    pd3dDevice->CreateVertexBuffer( dwNumVertices*sizeof(VERTEXBASIS), 0, 0, 0,
                                    &pBasisVB );

    // Fill the VB with the basis vectors
    VERTEXBASIS* pVertexBases;
    pBasisVB->Lock( 0, 0, (BYTE**)&pVertexBases, 0 );

    // Clear the basis vectors
    for( DWORD i = 0; i < dwNumVertices; i++)
    {
        pVertexBases[i].S = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
        pVertexBases[i].T = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    }

    DWORD dwNumPrimitives = 0L;
    if( dwPrimType == D3DPT_TRIANGLELIST )
        dwNumPrimitives = pIndices ? dwNumIndices / 3 : dwNumVertices / 3;
    if( dwPrimType == D3DPT_QUADLIST )
        dwNumPrimitives = pIndices ? dwNumIndices / 4 : dwNumVertices / 4;

    // Walk through the triangle list and calculate gradiants for each triangle.
    // Sum the results into the S and T components.
    for( i = 0; i < dwNumPrimitives; i++ )
    {
        if( dwPrimType == D3DPT_TRIANGLELIST )
        {
            DWORD i0 = pIndices ? pIndices[3*i+0] : 3*i+0;
            DWORD i1 = pIndices ? pIndices[3*i+1] : 3*i+1;
            DWORD i2 = pIndices ? pIndices[3*i+2] : 3*i+2;

            D3DVERTEX&   v0 = pVertices[i0];
            D3DVERTEX&   v1 = pVertices[i1];
            D3DVERTEX&   v2 = pVertices[i2];
            VERTEXBASIS& b0 = pVertexBases[i0];
            VERTEXBASIS& b1 = pVertexBases[i1];
            VERTEXBASIS& b2 = pVertexBases[i2];

            CreateBasisVectors( v0, v1, v2, b0, b1, b2 );
        }

        if( dwPrimType == D3DPT_QUADLIST )
        {
            DWORD i0 = pIndices ? pIndices[4*i+0] : 4*i+0;
            DWORD i1 = pIndices ? pIndices[4*i+1] : 4*i+1;
            DWORD i2 = pIndices ? pIndices[4*i+2] : 4*i+2;
            DWORD i3 = pIndices ? pIndices[4*i+3] : 4*i+3;

            D3DVERTEX&   v0 = pVertices[i0];
            D3DVERTEX&   v1 = pVertices[i1];
            D3DVERTEX&   v2 = pVertices[i2];
            D3DVERTEX&   v3 = pVertices[i3];
            VERTEXBASIS& b0 = pVertexBases[i0];
            VERTEXBASIS& b1 = pVertexBases[i1];
            VERTEXBASIS& b2 = pVertexBases[i2];
            VERTEXBASIS& b3 = pVertexBases[i3];

            CreateBasisVectors( v0, v1, v2, b0, b1, b2 );
            CreateBasisVectors( v0, v2, v3, b0, b2, b3 );
        }
    }

    // Calculate the SxT vector
    for( i = 0; i < dwNumVertices; i++ )
    {
        // Normalize the S, T vectors
        D3DXVec3Normalize( &pVertexBases[i].S, &pVertexBases[i].S );
        D3DXVec3Normalize( &pVertexBases[i].T, &pVertexBases[i].T );

        // Get the cross of the S and T vectors
        D3DXVec3Cross( &pVertexBases[i].SxT, &pVertexBases[i].S, &pVertexBases[i].T );

        // Need a normalized normal
        D3DXVECTOR3 vNormal;
        D3DXVec3Normalize( &vNormal, &pVertices[i].normal );

        // Get the direction of the SxT vector
        if( D3DXVec3Dot( &pVertexBases[i].SxT, &vNormal ) < 0.0f )
            pVertexBases[i].SxT = -pVertexBases[i].SxT;
    }

    pBasisVB->Unlock();
    return pBasisVB;
}




//-----------------------------------------------------------------------------
// Name: Create()
// Desc: Creates the geometry of a toothed-gear for the part
//-----------------------------------------------------------------------------
HRESULT CGearMesh::Create( LPDIRECT3DDEVICE8 pd3dDevice, FLOAT fRadius, 
                           FLOAT fRotationSpeed, DWORD dwNumTeeth,
                           FLOAT fRotationOffset )
{
    // Set parameters
    m_fRotationSpeed   = fRotationSpeed;
    m_fRotationAngle   = 0.0f;

    // Values used for calculating the geometry of the vertices
    FLOAT TEETH_DEPTH  = 0.1f;
    FLOAT fOuterRadius = fRadius + TEETH_DEPTH/2;
    FLOAT fInnerRadius = fRadius - TEETH_DEPTH/2;
    FLOAT fDepth       = 0.1f; //half-thickness of the gear
    FLOAT fTheta       = 0.0f;
    FLOAT fSubTheta    = ( 2*D3DX_PI / dwNumTeeth )/5;
    FLOAT fAngle;

    m_dwNumBodyVertices  = 4 + (8*dwNumTeeth);
    m_dwNumBodyIndices   = 24*dwNumTeeth;
    m_dwNumTeethVertices = 16*dwNumTeeth;

    // Create the geometry buffers
    pd3dDevice->CreateVertexBuffer( m_dwNumBodyVertices*sizeof(D3DVERTEX), 0, 0, 0,
                                    &m_pBodyVB );
    pd3dDevice->CreateVertexBuffer( m_dwNumTeethVertices*sizeof(D3DVERTEX), 0, 0, 0,
                                    &m_pTeethVB );
    pd3dDevice->CreateIndexBuffer( m_dwNumBodyIndices*sizeof(WORD), 
                                   D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, 
                                   D3DPOOL_DEFAULT, &m_pBodyIB );

    // Lock the geometry buffers so we can fill them
    D3DVERTEX* pBodyVertices;
    WORD*      pBodyIndices;
    D3DVERTEX* pTeethVertices;
    m_pBodyVB->Lock(  0, 0, (BYTE**)&pBodyVertices,  0 );
    m_pTeethVB->Lock( 0, 0, (BYTE**)&pTeethVertices, 0 );
    m_pBodyIB->Lock(  0, 0, (BYTE**)&pBodyIndices,   0 );

    // Create and fill in the vertices for the gear body
    D3DVERTEX* pVertex = pBodyVertices;

    D3DXVECTOR3 vfront = D3DXVECTOR3( 0.0f, fDepth, 0.0f );
    D3DXVECTOR3 vback  = D3DXVECTOR3( 0.0f,-fDepth, 0.0f );
    D3DXVECTOR3 nfront = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
    D3DXVECTOR3 nback  = D3DXVECTOR3( 0.0f,-1.0f, 0.0f );
    FILL_VERTEX( pVertex++, vfront, nfront, 0.5f, 0.5f ); 
    FILL_VERTEX( pVertex++, vback,  nback,  0.5f, 0.5f ); 

    for( DWORD i=0; i<dwNumTeeth; i++ )
    {
        fTheta    = (i*2*D3DX_PI) / dwNumTeeth;
        fSubTheta = ( 2*D3DX_PI / dwNumTeeth )/6;
        fTheta += fRotationOffset;
        
        fAngle = fTheta - 2 * fSubTheta;
        vfront = D3DXVECTOR3( fInnerRadius * cosf( fAngle ), +fDepth, fInnerRadius * sinf( fAngle ) );
        vback  = D3DXVECTOR3( fInnerRadius * cosf( fAngle ), -fDepth, fInnerRadius * sinf( fAngle ) );
        FILL_VERTEX( pVertex++, vfront, nfront, (1+vfront.x/fOuterRadius)/2, (1+vfront.z/fOuterRadius)/2 ); 
        FILL_VERTEX( pVertex++, vback,  nback,  (1+vback.x /fOuterRadius)/2, (1+ vback.z/fOuterRadius)/2 ); 

        fAngle = fTheta - 1 * fSubTheta;
        vfront = D3DXVECTOR3( fOuterRadius * cosf( fAngle ), +fDepth, fOuterRadius * sinf( fAngle ) );
        vback  = D3DXVECTOR3( fOuterRadius * cosf( fAngle ), -fDepth, fOuterRadius * sinf( fAngle ) );
        FILL_VERTEX( pVertex++, vfront, nfront, (1+vfront.x/fOuterRadius)/2, (1+vfront.z/fOuterRadius)/2 ); 
        FILL_VERTEX( pVertex++, vback,  nback,  (1+vback.x /fOuterRadius)/2, (1+ vback.z/fOuterRadius)/2 ); 

        fAngle = fTheta + 1 * fSubTheta;
        vfront = D3DXVECTOR3( fOuterRadius * cosf( fAngle ), +fDepth, fOuterRadius * sinf( fAngle ) );
        vback  = D3DXVECTOR3( fOuterRadius * cosf( fAngle ), -fDepth, fOuterRadius * sinf( fAngle ) );
        FILL_VERTEX( pVertex++, vfront, nfront, (1+vfront.x/fOuterRadius)/2, (1+vfront.z/fOuterRadius)/2 ); 
        FILL_VERTEX( pVertex++, vback,  nback,  (1+vback.x /fOuterRadius)/2, (1+ vback.z/fOuterRadius)/2 ); 

        fAngle = fTheta + 2 * fSubTheta;
        vfront = D3DXVECTOR3( fInnerRadius * cosf( fAngle ), +fDepth, fInnerRadius * sinf( fAngle ) );
        vback  = D3DXVECTOR3( fInnerRadius * cosf( fAngle ), -fDepth, fInnerRadius * sinf( fAngle ) );
        FILL_VERTEX( pVertex++, vfront, nfront, (1+vfront.x/fOuterRadius)/2, (1+vfront.z/fOuterRadius)/2 ); 
        FILL_VERTEX( pVertex++, vback,  nback,  (1+vback.x /fOuterRadius)/2, (1+ vback.z/fOuterRadius)/2 ); 
    }

    fAngle = fRotationOffset - 2 * fSubTheta;
    vfront = D3DXVECTOR3( fInnerRadius * cosf( fAngle ), +fDepth, fInnerRadius * sinf( fAngle ) );
    vback  = D3DXVECTOR3( fInnerRadius * cosf( fAngle ), -fDepth, fInnerRadius * sinf( fAngle ) );
    FILL_VERTEX( pVertex++, vfront, nfront, (1+vfront.x/fOuterRadius)/2, (1+vfront.z/fOuterRadius)/2 ); 
    FILL_VERTEX( pVertex++, vback,  nback,  (1+vback.x /fOuterRadius)/2, (1+ vback.z/fOuterRadius)/2 ); 

    // Create and fill in the indices for the gear body
    WORD* pIndex = pBodyIndices;

    for( i = 0; i < dwNumTeeth; i++ )
    {
        WORD v = (WORD)( 2 + 8*i );
        (*pIndex++) = v+0;  (*pIndex++) = v+6;  (*pIndex++) = 0; 
        (*pIndex++) = v+6;  (*pIndex++) = v+8;  (*pIndex++) = 0; 
        (*pIndex++) = v+7;  (*pIndex++) = v+1;  (*pIndex++) = 1; 
        (*pIndex++) = v+9;  (*pIndex++) = v+7;  (*pIndex++) = 1; 

        (*pIndex++) = v+0;  (*pIndex++) = v+2;  (*pIndex++) = v+6; 
        (*pIndex++) = v+2;  (*pIndex++) = v+4;  (*pIndex++) = v+6; 
        (*pIndex++) = v+7;  (*pIndex++) = v+3;  (*pIndex++) = v+1; 
        (*pIndex++) = v+7;  (*pIndex++) = v+5;  (*pIndex++) = v+3; 
    }

    // Create vertices for the gear teeth
    for( i=0; i<dwNumTeeth; i++ )
    {
        FLOAT fSubTheta0 = ( 2*D3DX_PI / dwNumTeeth )/6;
        FLOAT fTheta0   = fRotationOffset + ((i+0)*2*D3DX_PI) / dwNumTeeth;
        FLOAT fTheta1   = fRotationOffset + ((i+1)*2*D3DX_PI) / dwNumTeeth;

        FLOAT fAngle0 = fTheta0 - 2 * fSubTheta0;
        FLOAT fAngle1 = fTheta0 - 1 * fSubTheta0;
        FLOAT fAngle2 = fTheta0 + 1 * fSubTheta0;
        FLOAT fAngle3 = fTheta0 + 2 * fSubTheta0;
        FLOAT fAngle4 = fTheta1 - 2 * fSubTheta0;

        D3DXVECTOR3 v0front = D3DXVECTOR3( fInnerRadius * cosf( fAngle0 ), fDepth, fInnerRadius * sinf( fAngle0 ) );
        D3DXVECTOR3 v0back  = D3DXVECTOR3( fInnerRadius * cosf( fAngle0 ),-fDepth, fInnerRadius * sinf( fAngle0 ) );
        D3DXVECTOR3 v1front = D3DXVECTOR3( fOuterRadius * cosf( fAngle1 ), fDepth, fOuterRadius * sinf( fAngle1 ) );
        D3DXVECTOR3 v1back  = D3DXVECTOR3( fOuterRadius * cosf( fAngle1 ),-fDepth, fOuterRadius * sinf( fAngle1 ) );
        D3DXVECTOR3 v2front = D3DXVECTOR3( fOuterRadius * cosf( fAngle2 ), fDepth, fOuterRadius * sinf( fAngle2 ) );
        D3DXVECTOR3 v2back  = D3DXVECTOR3( fOuterRadius * cosf( fAngle2 ),-fDepth, fOuterRadius * sinf( fAngle2 ) );
        D3DXVECTOR3 v3front = D3DXVECTOR3( fInnerRadius * cosf( fAngle3 ), fDepth, fInnerRadius * sinf( fAngle3 ) );
        D3DXVECTOR3 v3back  = D3DXVECTOR3( fInnerRadius * cosf( fAngle3 ),-fDepth, fInnerRadius * sinf( fAngle3 ) );
        D3DXVECTOR3 v4front = D3DXVECTOR3( fInnerRadius * cosf( fAngle4 ), fDepth, fInnerRadius * sinf( fAngle4 ) );
        D3DXVECTOR3 v4back  = D3DXVECTOR3( fInnerRadius * cosf( fAngle4 ),-fDepth, fInnerRadius * sinf( fAngle4 ) );

        D3DXVECTOR3 n0 = D3DXVECTOR3( cosf( fTheta0-0.5f ),        0.0f, sinf( fTheta0-0.5f ) );
        D3DXVECTOR3 n1 = D3DXVECTOR3( cosf( fTheta0 ),             0.0f, sinf( fTheta0 ) );
        D3DXVECTOR3 n2 = D3DXVECTOR3( cosf( fTheta0+0.5f ),        0.0f, sinf( fTheta0+0.5f ) );
        D3DXVECTOR3 n3 = D3DXVECTOR3( cosf( (fTheta0+fTheta1)/2 ), 0.0f, sinf( (fTheta0+fTheta1)/2 ) );

        FLOAT tu0 = ( 0.50f * fAngle0 );
        FLOAT tu1 = ( 0.50f * fAngle1 );
        FLOAT tu2 = ( 1.50f * fAngle1 );
        FLOAT tu3 = ( 1.50f * fAngle2 );
        FLOAT tu4 = ( 0.50f * fAngle2 );
        FLOAT tu5 = ( 0.50f * fAngle3 );
        FLOAT tu6 = ( 1.50f * fAngle3 );
        FLOAT tu7 = ( 1.50f * fAngle4 );

        FILL_VERTEX( pTeethVertices++, v0front, n0, tu0, 0.00f ); 
        FILL_VERTEX( pTeethVertices++, v0back,  n0, tu0, 0.14f ); 
        FILL_VERTEX( pTeethVertices++, v1back,  n0, tu1, 0.14f ); 
        FILL_VERTEX( pTeethVertices++, v1front, n0, tu1, 0.00f ); 

        FILL_VERTEX( pTeethVertices++, v1front, n1, tu2, 0.00f ); 
        FILL_VERTEX( pTeethVertices++, v1back,  n1, tu2, 0.14f ); 
        FILL_VERTEX( pTeethVertices++, v2back,  n1, tu3, 0.14f ); 
        FILL_VERTEX( pTeethVertices++, v2front, n1, tu3, 0.00f ); 

        FILL_VERTEX( pTeethVertices++, v2front, n2, tu4, 0.00f ); 
        FILL_VERTEX( pTeethVertices++, v2back,  n2, tu4, 0.14f ); 
        FILL_VERTEX( pTeethVertices++, v3back,  n2, tu5, 0.14f ); 
        FILL_VERTEX( pTeethVertices++, v3front, n2, tu5, 0.00f ); 

        FILL_VERTEX( pTeethVertices++, v3front, n3, tu6, 0.00f ); 
        FILL_VERTEX( pTeethVertices++, v3back,  n3, tu6, 0.14f ); 
        FILL_VERTEX( pTeethVertices++, v4back,  n3, tu7, 0.14f ); 
        FILL_VERTEX( pTeethVertices++, v4front, n3, tu7, 0.00f ); 
    }

    m_pBodyVB->Unlock();
    m_pTeethVB->Unlock();
    m_pBodyIB->Unlock();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Animate the gear
//-----------------------------------------------------------------------------
HRESULT CGearMesh::FrameMove( LPDIRECT3DDEVICE8 pd3dDevice, FLOAT fTime )
{
    m_fRotationAngle = fTime * m_fRotationSpeed;
    D3DXMatrixRotationY( &m_matRotation, m_fRotationAngle );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Animate the gear
//-----------------------------------------------------------------------------
HRESULT CGearMesh::Render( LPDIRECT3DDEVICE8 pd3dDevice )
{
    // Orient the geoemtry
    D3DXMATRIX mat, matWorldSaved;
    pd3dDevice->GetTransform( D3DTS_WORLD, &matWorldSaved );
    D3DXMatrixMultiply( &mat, &m_matRotation, &m_matPosition );
    D3DXMatrixMultiply( &mat, &mat, &matWorldSaved );
    pd3dDevice->SetTransform( D3DTS_WORLD, &mat );
    
    // Draw the gear body
    pd3dDevice->SetIndices( m_pBodyIB, 0 );
    pd3dDevice->SetStreamSource( 0, m_pBodyVB,      sizeof(D3DVERTEX) );
    pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, m_dwNumBodyVertices, 
                                        0, m_dwNumBodyIndices/3 );
    // Draw the gear teeth edges
    pd3dDevice->SetStreamSource( 0, m_pTeethVB,      sizeof(D3DVERTEX) );
    pd3dDevice->DrawVertices( D3DPT_QUADLIST, 0, m_dwNumTeethVertices );

    // Restore the world matrix
    pd3dDevice->SetTransform( D3DTS_WORLD, &matWorldSaved );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Create()
// Desc: Creates the geometry of a toothed-gear for the part
//-----------------------------------------------------------------------------
HRESULT CBumpyGearMesh::Create( LPDIRECT3DDEVICE8 pd3dDevice, FLOAT fRadius, 
                                FLOAT fRotationSpeed, DWORD dwNumTeeth,
                                FLOAT fRotationOffset )
{
    // Create the mesh geometry
    CGearMesh::Create( pd3dDevice, fRadius, fRotationSpeed, dwNumTeeth, fRotationOffset );

    // Create the basis vectors
    D3DVERTEX* pBodyVertices;
    WORD*      pBodyIndices;
    D3DVERTEX* pTeethVertices;
    m_pBodyVB->Lock(  0, 0, (BYTE**)&pBodyVertices,  0 );
    m_pTeethVB->Lock( 0, 0, (BYTE**)&pTeethVertices, 0 );
    m_pBodyIB->Lock(  0, 0, (BYTE**)&pBodyIndices,   0 );

    m_pBodyBasisVB  = CreateBasisVectorsVB( pd3dDevice, D3DPT_TRIANGLELIST, 
                                            pBodyVertices, m_dwNumBodyVertices, 
                                            pBodyIndices, m_dwNumBodyIndices );
    m_pTeethBasisVB = CreateBasisVectorsVB( pd3dDevice, D3DPT_QUADLIST, 
                                            pTeethVertices, m_dwNumTeethVertices, 
                                            NULL, 0L );

    m_pBodyVB->Unlock();
    m_pTeethVB->Unlock();
    m_pBodyIB->Unlock();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Animate the gear
//-----------------------------------------------------------------------------
HRESULT CBumpyGearMesh::Render( LPDIRECT3DDEVICE8 pd3dDevice )
{
    // Orient the geoemtry
    D3DXMATRIX mat, matWorldSaved;
    pd3dDevice->GetTransform( D3DTS_WORLD, &matWorldSaved );
    D3DXMatrixMultiply( &mat, &m_matRotation, &m_matPosition );
    D3DXMatrixMultiply( &mat, &mat, &matWorldSaved );
    pd3dDevice->SetTransform( D3DTS_WORLD, &mat );
    
    // Note: when passing matrices to a vertex shader, we transpose them, since
    // matrix multiplies are done with dot product operations on the matrix rows.
    D3DXMATRIX matT;
    D3DXMATRIX matWorld, matView, matProj;
    pd3dDevice->GetTransform( D3DTS_WORLD,      &matWorld );
    pd3dDevice->GetTransform( D3DTS_VIEW,       &matView );
    pd3dDevice->GetTransform( D3DTS_PROJECTION, &matProj );

    // Pass the world matrix to the vertex shader
    D3DXMatrixTranspose( &matT, &matWorld );
    pd3dDevice->SetVertexShaderConstant( 0, &matT, 4 );
    
    // Pass the view * projection matrix to the vertex shader
    D3DXMatrixMultiply( &mat, &matView, &matProj );
    D3DXMatrixTranspose( &matT, &mat );
    pd3dDevice->SetVertexShaderConstant( 4, &matT, 4 );

    // Pass the world space view position to the vertex shader.
    D3DXMATRIX matViewInverse;
    D3DXMatrixInverse( &matViewInverse, 0, &matView );
    D3DXVECTOR4 v4WorldViewPos(0.0f,0.0f,0.0f,1.0f);
    D3DXVec4Transform( &v4WorldViewPos, &v4WorldViewPos, &matViewInverse );
    pd3dDevice->SetVertexShaderConstant( 8, &v4WorldViewPos, 1 );

    D3DXVECTOR4 fTestValues( 0.0f, 0.0f, 1.0f, 0.0f );
    pd3dDevice->SetVertexShaderConstant( 9, fTestValues, 1 );
    
    // Draw the gear body
    pd3dDevice->SetIndices( m_pBodyIB, 0 );
    pd3dDevice->SetStreamSource( 0, m_pBodyVB,      sizeof(D3DVERTEX) );
    pd3dDevice->SetStreamSource( 1, m_pBodyBasisVB, sizeof(VERTEXBASIS) );
    pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, m_dwNumBodyVertices, 
                                        0, m_dwNumBodyIndices/3 );

    // Draw the gear teeth edges
    pd3dDevice->SetStreamSource( 0, m_pTeethVB,      sizeof(D3DVERTEX) );
    pd3dDevice->SetStreamSource( 1, m_pTeethBasisVB, sizeof(VERTEXBASIS) );
    pd3dDevice->DrawVertices( D3DPT_QUADLIST, 0, m_dwNumTeethVertices );

    // Restore the world matrix
    pd3dDevice->SetTransform( D3DTS_WORLD, &matWorldSaved );

    return S_OK;
}



