//-----------------------------------------------------------------------------
// File: Mesh.cpp
//
// Desc: Mesh objects
//
// Hist: 04.10.01 - New for May XDK release 
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "Mesh.h"
#include "File.h"
#include "Globals.h"

#if defined(_XBOX)

#include <XbUtil.h>

#endif




//-----------------------------------------------------------------------------
// Global mesh lists
//-----------------------------------------------------------------------------
DWORD g_D3DPrimTypeConstants[] =
{
    D3DPT_POINTLIST,
    D3DPT_LINELIST,
    D3DPT_LINESTRIP,
    D3DPT_TRIANGLELIST,
    D3DPT_TRIANGLESTRIP,
    D3DPT_TRIANGLEFAN,
    D3DPT_FORCE_DWORD
};

const INT MAX_VIS_INDEX = 6;
D3DMATERIAL8 g_VisualizationColors[ MAX_VIS_INDEX ] =
{
    {
        { 1.0f, 0.0f, 0.0f, 1.0f },
        { 1.0f, 0.0f, 0.0f, 1.0f },
        { 0.0f, 0.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f, 0.0f },
        0.0f,
    },

    {
        { 0.0f, 1.0f, 0.0f, 1.0f },
        { 0.0f, 1.0f, 0.0f, 1.0f },
        { 0.0f, 0.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f, 0.0f },
        0.0f,
    },

    {
        { 0.0f, 0.0f, 1.0f, 1.0f },
        { 0.0f, 0.0f, 1.0f, 1.0f },
        { 0.0f, 0.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f, 0.0f },
        0.0f,
    },

    {
        { 0.0f, 1.0f, 1.0f, 1.0f },
        { 0.0f, 1.0f, 1.0f, 1.0f },
        { 0.0f, 0.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f, 0.0f },
        0.0f,
    },

    {
        { 1.0f, 0.0f, 1.0f, 1.0f },
        { 1.0f, 0.0f, 1.0f, 1.0f },
        { 0.0f, 0.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f, 0.0f },
        0.0f,
    },

    {
        { 1.0f, 1.0f, 0.0f, 1.0f },
        { 1.0f, 1.0f, 0.0f, 1.0f },
        { 0.0f, 0.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f, 0.0f },
        0.0f,
    },
};




//-----------------------------------------------------------------------------
// Local structs
//-----------------------------------------------------------------------------
struct MeshElementData // for writing to file
{
    DWORD dwPrimitives;
    DWORD dwPrimType;
    DWORD dwIndexBufferCount;
    DWORD dwNumVertices;
    DWORD dwVertexSize;
    DWORD dwFVF;
    DWORD dwBasisVectorOffset;
};




//-----------------------------------------------------------------------------
// Name: MeshElement()
// Desc: Construct an empty element
//-----------------------------------------------------------------------------
MeshElement::MeshElement()
:
    m_pShader           ( NULL ),
    m_uNumPrimitives    ( 0 ),
    m_PrimType          ( D3DPT_TRIANGLELIST ),
    m_uIndexBufferCount ( 0 ),
    m_pIndexBuffer      ( NULL ),
    m_dwFVF             ( 0 ),
    m_iBasisVectorOffset( 0 ),
    m_uNumVertices      ( 0 ),
    m_uVertexSize       ( 0 ),
    m_pVertexBuffer     ( NULL )
{
}




//-----------------------------------------------------------------------------
// Name: MeshElement()
// Desc: Create a mesh element using an FVF code
//-----------------------------------------------------------------------------
MeshElement::MeshElement( DWORD dwFVF, UINT uNumVerts, UINT uNumIndices )
:
    m_pShader           ( NULL ),
    m_uNumPrimitives    ( 0 ),
    m_PrimType          ( D3DPT_TRIANGLELIST ),
    m_uIndexBufferCount ( uNumIndices ),
    m_pIndexBuffer      ( NULL ),
    m_dwFVF             ( dwFVF ),
    m_iBasisVectorOffset( 0 ),
    m_uNumVertices      ( uNumVerts ),
    m_uVertexSize       ( D3DXGetFVFVertexSize( dwFVF ) ),
    m_pVertexBuffer     ( NULL )
{
    HRESULT hr = g_pd3dDevice->CreateVertexBuffer( m_uVertexSize * m_uNumVertices, 
                                                   D3DUSAGE_WRITEONLY, 0, 
                                                   D3DPOOL_MANAGED, &m_pVertexBuffer);
    assert( hr == S_OK );

    hr = g_pd3dDevice->CreateIndexBuffer( sizeof(WORD) * m_uIndexBufferCount, 
                                          D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, 
                                          D3DPOOL_MANAGED, &m_pIndexBuffer );
    assert( hr == S_OK );
}



//-----------------------------------------------------------------------------
// Name: ~MeshElement()
// Desc: Destroy mesh lement
//-----------------------------------------------------------------------------
MeshElement::~MeshElement()
{
    SAFE_RELEASE( m_pIndexBuffer );
    SAFE_RELEASE( m_pVertexBuffer );
}




//-----------------------------------------------------------------------------
// Name: Save()
// Desc: Write the mesh element to the file
//-----------------------------------------------------------------------------
HRESULT MeshElement::Save( const File& file )
{
    assert( m_pShader != NULL );

    // Save shader type and shader data
    DWORD dwShaderType = m_pShader->Type();
    if( !file.Write( &dwShaderType, sizeof(dwShaderType) ) )
        return E_FAIL;
    m_pShader->Save( file );

    MeshElementData data;
    data.dwPrimitives        = m_uNumPrimitives;
    data.dwPrimType          = ConstantToIndex( m_PrimType, g_D3DPrimTypeConstants );
    data.dwIndexBufferCount  = m_uIndexBufferCount;
    data.dwNumVertices       = m_uNumVertices;
    data.dwVertexSize        = m_uVertexSize;
    data.dwFVF               = m_dwFVF;
    data.dwBasisVectorOffset = m_iBasisVectorOffset;
    if( !file.Write( &data, sizeof(data) ) )
        return E_FAIL;

    // Save indices
    DWORD dwIndexBufferSize = sizeof(WORD) * m_uIndexBufferCount;
    BYTE* pIndices;
    m_pIndexBuffer->Lock( 0, dwIndexBufferSize, &pIndices, D3DLOCK_READONLY );
    if( !file.Write( pIndices, dwIndexBufferSize ) )
        return E_FAIL;
    m_pIndexBuffer->Unlock();

    // Save vertices
    DWORD dwVertexBufferSize = m_uVertexSize * m_uNumVertices;
    BYTE* pVertices;
    m_pVertexBuffer->Lock( 0, dwVertexBufferSize, &pVertices, D3DLOCK_READONLY );
    if( !file.Write( pVertices, dwVertexBufferSize ) )
        return E_FAIL;
    m_pVertexBuffer->Unlock();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Load()
// Desc: Load the mesh element from the file
//-----------------------------------------------------------------------------
HRESULT MeshElement::Load( const File& file )
{
    DWORD dwBytesRead;

    // Load the shader
    DWORD dwShaderType;
    if( !file.Read( &dwShaderType, sizeof(dwShaderType), dwBytesRead ) )
        return E_FAIL;

    m_pShader = Shader::CreateShaderOfType( dwShaderType );
    m_pShader->Load( file );

    // Read basic mesh element data
    MeshElementData data;
    if( !file.Read( &data, sizeof(data), dwBytesRead ) )
        return E_FAIL;

    m_uNumPrimitives     = data.dwPrimitives;
    m_PrimType           = D3DPRIMITIVETYPE( IndexToConstant( data.dwPrimType, 
                                             g_D3DPrimTypeConstants ) );
    m_uIndexBufferCount  = data.dwIndexBufferCount;
    m_uNumVertices       = data.dwNumVertices;
    m_uVertexSize        = data.dwVertexSize;
    m_dwFVF              = data.dwFVF;
    m_iBasisVectorOffset = data.dwBasisVectorOffset;

    // Load indices
    DWORD dwIndexBufferSize = sizeof(WORD) * m_uIndexBufferCount;
    HRESULT hr = g_pd3dDevice->CreateIndexBuffer( dwIndexBufferSize, 
                                                  D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, 
                                                  D3DPOOL_MANAGED, &m_pIndexBuffer);

    assert( hr == D3D_OK );
    BYTE* pIndices;
    m_pIndexBuffer->Lock(0, dwIndexBufferSize, &pIndices, D3DLOCK_READONLY);
    if( !file.Read( pIndices, dwIndexBufferSize, dwBytesRead ) )
        return E_FAIL;

    m_pIndexBuffer->Unlock();

    // Load vertices
    DWORD dwVertexBufferSize = m_uVertexSize * m_uNumVertices;
    hr = g_pd3dDevice->CreateVertexBuffer( dwVertexBufferSize, D3DUSAGE_WRITEONLY, 
                                           0, D3DPOOL_MANAGED, &m_pVertexBuffer );

    assert(hr == D3D_OK);
    BYTE* pVertices;
    m_pVertexBuffer->Lock(0, dwVertexBufferSize, &pVertices, D3DLOCK_READONLY);
    if( !file.Read( pVertices, dwVertexBufferSize, dwBytesRead ) )
        return E_FAIL;

    m_pVertexBuffer->Unlock();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Load()
// Desc: Render the mesh element
//-----------------------------------------------------------------------------
VOID MeshElement::Render()
{
    g_pd3dDevice->SetIndices( m_pIndexBuffer, 0 );
    g_pd3dDevice->SetStreamSource( 0, m_pVertexBuffer, m_uVertexSize );

    g_pd3dDevice->SetRenderState( D3DRS_FILLMODE, g_bWireframe ? D3DFILL_WIREFRAME :
                                                                 D3DFILL_SOLID );

    INT iNumPasses = m_pShader->GetNumPasses();
    for( INT iPass = 0; iPass < iNumPasses; ++iPass )
    {
        // Output the shader
        m_pShader->Output( iPass );

        if( g_bVisualizeStrips )
        {
            INT iVisIndex = 0;

            UINT uIndexStart = 0;
            UINT uIndexEnd;

            UINT uIndexBufferSize = sizeof(WORD) * m_uIndexBufferCount;
            WORD* pIndices = NULL;
            m_pIndexBuffer->Lock( 0, uIndexBufferSize, (BYTE**)&pIndices, 
                                  D3DLOCK_READONLY );

            while( uIndexStart < m_uIndexBufferCount )
            {
                // Find end of strip
                for( uIndexEnd = uIndexStart+2; 
                     uIndexEnd < m_uIndexBufferCount-1; ++uIndexEnd )
                {
                    // Check for duplicate verts
                    if( pIndices[uIndexEnd+1] == pIndices[uIndexEnd] )
                        break;
                }

                // Draw strip
                g_pd3dDevice->SetMaterial( &g_VisualizationColors[iVisIndex++] );
                if( iVisIndex >= MAX_VIS_INDEX )
                    iVisIndex = 0;

                g_pd3dDevice->DrawIndexedPrimitive( m_PrimType, 0, m_uNumVertices, 
                                                    uIndexStart, uIndexEnd-uIndexStart-1 );

                if( (uIndexEnd-uIndexStart-1) & 1 )
                    uIndexStart = uIndexEnd+2;
                else
                    uIndexStart = uIndexEnd+3;
            }

            m_pIndexBuffer->Unlock();
        }
        else
        {
            // Draw the strip
            g_pd3dDevice->DrawIndexedPrimitive( m_PrimType, 0, m_uNumVertices, 
                                                0, m_uNumPrimitives );
        }
    }

    if( g_bShowNormals )
    {
        UINT uVertexBufferSize = m_uVertexSize * m_uNumVertices;
        BYTE* pVertices;
        m_pVertexBuffer->Lock(0, uVertexBufferSize, &pVertices, D3DLOCK_READONLY);

        INT iNormalOffset = ComputeFVFOffset(D3DFVF_NORMAL, m_dwFVF);

        g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
        g_pd3dDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL );
        g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
        g_pd3dDevice->SetVertexShader( D3DFVF_XYZ | D3DFVF_DIFFUSE );
        g_pd3dDevice->SetPixelShader( 0 );
        g_pd3dDevice->SetTexture( 0, 0 );
        g_pd3dDevice->SetTexture( 1, 0 );
        g_pd3dDevice->SetTexture( 2, 0 );
        g_pd3dDevice->SetTexture( 3, 0 );

        for( UINT i = 0; i < m_uNumVertices; ++i )
        {
            D3DXVECTOR3* pPos = (D3DXVECTOR3*)( pVertices + i * m_uVertexSize );
            D3DXVECTOR3* pNormal = (D3DXVECTOR3*)( pVertices + 
                                                   i * m_uVertexSize + iNormalOffset);
            
            struct
            { 
                D3DVECTOR pos;
                D3DCOLOR color;
            }
            LinePnts[2];
            
            LinePnts[0].pos = *pPos;
            LinePnts[0].color = 0xffffffff;
            LinePnts[1].pos = *pPos + *pNormal;
            LinePnts[1].color = 0xffffffff;

            g_pd3dDevice->DrawPrimitiveUP( D3DPT_LINELIST, 1, LinePnts, 
                                           sizeof(D3DXVECTOR3) );
        }

        m_pVertexBuffer->Unlock();
    }

    if( g_bShowBasisVectors )
    {
        UINT uVertexBufferSize = m_uVertexSize * m_uNumVertices;
        BYTE* pVertices;
        m_pVertexBuffer->Lock(0, uVertexBufferSize, &pVertices, D3DLOCK_READONLY);

        g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
        g_pd3dDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL );
        g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
        g_pd3dDevice->SetVertexShader( D3DFVF_XYZ | D3DFVF_DIFFUSE );
        g_pd3dDevice->SetPixelShader( 0 );
        g_pd3dDevice->SetTexture( 0, 0 );
        g_pd3dDevice->SetTexture( 1, 0 );
        g_pd3dDevice->SetTexture( 2, 0 );
        g_pd3dDevice->SetTexture( 3, 0 );

        for( UINT i = 0; i < m_uNumVertices; ++i )
        {
            D3DXVECTOR3* pPos = (D3DXVECTOR3*)( pVertices + i * m_uVertexSize );
            D3DXVECTOR3* pBV = (D3DXVECTOR3*)( pVertices + i * m_uVertexSize + 
                                               m_iBasisVectorOffset );
            
            struct
            { 
                D3DVECTOR pos;
                D3DCOLOR color;
            }
            LinePnts[2];
            
            LinePnts[0].pos = *pPos;
            LinePnts[0].color = 0xffff0000;
            LinePnts[1].pos = *pPos + pBV[0];
            LinePnts[1].color = 0xffff0000;
            g_pd3dDevice->DrawPrimitiveUP( D3DPT_LINELIST, 1, LinePnts, 
                                           sizeof(LinePnts[0]) );

            LinePnts[0].pos = *pPos;
            LinePnts[0].color = 0xff00ff00;
            LinePnts[1].pos = *pPos + pBV[1];
            LinePnts[1].color = 0xff00ff00;
            g_pd3dDevice->DrawPrimitiveUP( D3DPT_LINELIST, 1, LinePnts, 
                                           sizeof(LinePnts[0]) );

            LinePnts[0].pos = *pPos;
            LinePnts[0].color = 0xff0000ff;
            LinePnts[1].pos = *pPos + pBV[2];
            LinePnts[1].color = 0xff0000ff;
            g_pd3dDevice->DrawPrimitiveUP( D3DPT_LINELIST, 1, LinePnts, 
                                           sizeof(LinePnts[0]) );
        }
        m_pVertexBuffer->Unlock();
    }
}




//-----------------------------------------------------------------------------
// Name: AddBasisVectors()
// Desc: Allocate space for basis vectors
//-----------------------------------------------------------------------------
VOID MeshElement::AddBasisVectors()
{
    UINT uOldVertexSize = m_uVertexSize;
    LPDIRECT3DVERTEXBUFFER8 pOldVertexBuffer = m_pVertexBuffer;
    UINT uOldVertexBufferSize = m_uVertexSize * m_uNumVertices;

    m_uVertexSize += sizeof(D3DXVECTOR3) * 3;

    UINT uVertexBufferSize = m_uVertexSize * m_uNumVertices;

    // Allocate space for basis vectors in vertices
    HRESULT hr = g_pd3dDevice->CreateVertexBuffer( uVertexBufferSize, 
                                                   D3DUSAGE_WRITEONLY, 
                                                   0, D3DPOOL_MANAGED,
                                                   &m_pVertexBuffer );
    assert( hr == S_OK );
    USED( hr );

    INT iUVOffset = ComputeFVFOffset( D3DFVF_TEX1, m_dwFVF );
    INT iBVOffset = uOldVertexSize;

    BYTE *pOldVertices;
    BYTE *pVertices;
    pOldVertexBuffer->Lock( 0, uOldVertexBufferSize, &pOldVertices, D3DLOCK_READONLY );
    m_pVertexBuffer->Lock( 0, uVertexBufferSize, &pVertices, D3DLOCK_READONLY );

    // Copy vertex data over
    for( UINT i = 0; i < m_uNumVertices; ++i )
    {
        memcpy( pVertices + i * m_uVertexSize, 
                pOldVertices + i * uOldVertexSize, uOldVertexSize);
    }

    pOldVertexBuffer->Unlock();
    m_pVertexBuffer->Unlock();

    pOldVertexBuffer->Release();

    // Calculate basis vectors
    CalculateBasisVectors( iUVOffset, iBVOffset );
    m_iBasisVectorOffset = iBVOffset;
}




//-----------------------------------------------------------------------------
// Name: AddBasisVectors()
// Desc: Calculate basis vectors for the mesh element
//-----------------------------------------------------------------------------
VOID MeshElement::CalculateBasisVectors( INT iUVOffset, INT iBVOffset )
{
    static const FLOAT SMALL_FLOAT = 1e-12f;

    INT iNormalOffset = ComputeFVFOffset(D3DFVF_NORMAL, m_dwFVF);

    UINT uVertexBufferSize = m_uVertexSize * m_uNumVertices;
    BYTE* pVertices;
    m_pVertexBuffer->Lock( 0, uVertexBufferSize, &pVertices, D3DLOCK_READONLY );

    struct BasisVector
    { 
        D3DXVECTOR3 S;
        D3DXVECTOR3 T;
        D3DXVECTOR3 SxT;
    };
    
    // Clear the basis vectors
    for( UINT j = 0; j < m_uNumVertices; ++j )
    {
        BasisVector* pBasis = (BasisVector*)(pVertices + j*m_uVertexSize + iBVOffset);

        pBasis->S = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
        pBasis->T = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
    }

    // Walk through the triangle list and calculate gradiants for each triangle.
    // Sum the results into the S and T components.
    UINT uIndexBufferSize = sizeof(WORD) * m_uIndexBufferCount;
    WORD *pIndices;
    m_pIndexBuffer->Lock(0, uIndexBufferSize, (BYTE**)&pIndices, D3DLOCK_READONLY);

    UINT i = 0;
    INT ind0 = 0;
    INT ind1 = 0;
    INT ind2 = 0;

    if( D3DPT_TRIANGLELIST == m_PrimType )
    {
        i = 0;
    }
    else if( D3DPT_TRIANGLESTRIP == m_PrimType )
    {
        ind0 = -1;
        ind1 = pIndices[0];
        ind2 = pIndices[1];
        i = 2;
    }

    while( i < m_uIndexBufferCount )
    {
        BYTE *pVert0 = NULL;
        BYTE *pVert1 = NULL;
        BYTE *pVert2 = NULL;

        if( D3DPT_TRIANGLELIST == m_PrimType )
        {
            pVert0 = pVertices + pIndices[i+0] * m_uVertexSize;
            pVert1 = pVertices + pIndices[i+1] * m_uVertexSize;
            pVert2 = pVertices + pIndices[i+2] * m_uVertexSize;
            i += 3;
        }
        else if( D3DPT_TRIANGLESTRIP == m_PrimType )
        {
            ind0 = ind1;
            ind1 = ind2;
            ind2 = pIndices[i];
            
            if( ind0 != ind1 && ind1 != ind2 && ind2 != ind0 )
            {
                if( i & 1 )
                {
                    pVert0 = pVertices + ind2 * m_uVertexSize;
                    pVert1 = pVertices + ind1 * m_uVertexSize;
                    pVert2 = pVertices + ind0 * m_uVertexSize;
                }
                else
                {
                    pVert0 = pVertices + ind0 * m_uVertexSize;
                    pVert1 = pVertices + ind1 * m_uVertexSize;
                    pVert2 = pVertices + ind2 * m_uVertexSize;
                }

                ++i;
            }
            else
            {
                // Degenerate triangle
                ++i;
                continue;
            }
        }
        else
        {
            // Unsupported primitive type
            assert(0);
        }

        D3DXVECTOR3* pPos0 = (D3DXVECTOR3*)pVert0;
        D3DXVECTOR3* pPos1 = (D3DXVECTOR3*)pVert1;
        D3DXVECTOR3* pPos2 = (D3DXVECTOR3*)pVert2;

        D3DXVECTOR2* pTC0 = (D3DXVECTOR2*)(pVert0 + iUVOffset);
        D3DXVECTOR2* pTC1 = (D3DXVECTOR2*)(pVert1 + iUVOffset);
        D3DXVECTOR2* pTC2 = (D3DXVECTOR2*)(pVert2 + iUVOffset);

        BasisVector* pBV0 = (BasisVector*)(pVert0 + iBVOffset);
        BasisVector* pBV1 = (BasisVector*)(pVert1 + iBVOffset);
        BasisVector* pBV2 = (BasisVector*)(pVert2 + iBVOffset);

        FLOAT ds1 = pTC1->x - pTC0->x;
        FLOAT dt1 = pTC1->y - pTC0->y;

        FLOAT ds2 = pTC2->x - pTC0->x;
        FLOAT dt2 = pTC2->y - pTC0->y;

        // x, s, t
        D3DXVECTOR3 edge01 = D3DXVECTOR3( pPos1->x - pPos0->x, ds1, dt1 );
        D3DXVECTOR3 edge02 = D3DXVECTOR3( pPos2->x - pPos0->x, ds2, dt2 );

        D3DXVECTOR3 cp;
        D3DXVec3Cross( &cp, &edge01, &edge02 );
        if( fabs(cp.x) > SMALL_FLOAT )
        {
            FLOAT dsdx = -cp.y / cp.x;
            FLOAT dtdx = -cp.z / cp.x;

            pBV0->S.x += dsdx;
            pBV0->T.x += dtdx;

            pBV1->S.x += dsdx;
            pBV1->T.x += dtdx;

            pBV2->S.x += dsdx;
            pBV2->T.x += dtdx;
        }

        // y, s, t
        edge01 = D3DXVECTOR3( pPos1->y - pPos0->y, ds1, dt1 );
        edge02 = D3DXVECTOR3( pPos2->y - pPos0->y, ds2, dt2 );

        D3DXVec3Cross( &cp, &edge01, &edge02 );
        if( fabs(cp.x) > SMALL_FLOAT )
        {
            FLOAT dsdx = -cp.y / cp.x;
            FLOAT dtdx = -cp.z / cp.x;

            pBV0->S.y += dsdx;
            pBV0->T.y += dtdx;

            pBV1->S.y += dsdx;
            pBV1->T.y += dtdx;

            pBV2->S.y += dsdx;
            pBV2->T.y += dtdx;
        }

        // z, s, t
        edge01 = D3DXVECTOR3( pPos1->z - pPos0->z, ds1, dt1 );
        edge02 = D3DXVECTOR3( pPos2->z - pPos0->z, ds2, dt2 );

        D3DXVec3Cross( &cp, &edge01, &edge02 );
        if( fabs(cp.x) > SMALL_FLOAT )
        {
            FLOAT dsdx = -cp.y / cp.x;
            FLOAT dtdx = -cp.z / cp.x;

            pBV0->S.z += dsdx;
            pBV0->T.z += dtdx;

            pBV1->S.z += dsdx;
            pBV1->T.z += dtdx;

            pBV2->S.z += dsdx;
            pBV2->T.z += dtdx;
        }
    }

    m_pIndexBuffer->Unlock();

    // Calculate the SxT vector
    for( i = 0; i < m_uNumVertices; ++i )
    {
        BasisVector* pBasis = (BasisVector*)(pVertices + i * m_uVertexSize + iBVOffset);

        // Normalize the S, T vectors
        D3DXVec3Normalize( &pBasis->S, &pBasis->S );
        D3DXVec3Normalize( &pBasis->T, &pBasis->T );

        // Get the cross of the S and T vectors
        D3DXVec3Cross( &pBasis->SxT, &pBasis->S, &pBasis->T );

        // v coordinates go in opposite direction from the texture v increase in xyz
        //pBasis->T = -pBasis->T;

        D3DXVECTOR3* pNormal = (D3DXVECTOR3*)( pVertices + i * m_uVertexSize + 
                                               iNormalOffset );

        // Get the direction of the SxT vector
        if( D3DXVec3Dot( &pBasis->SxT, pNormal ) < 0.0f )
            pBasis->SxT = -pBasis->SxT;
    }

    m_pVertexBuffer->Unlock();
}



//-----------------------------------------------------------------------------
// Name: ConstantToIndex()
// Desc: Returns the index of the given constant
//-----------------------------------------------------------------------------
DWORD MeshElement::ConstantToIndex( DWORD dwConst, const DWORD* pTable ) // static
{
    assert( pTable != NULL );
    for( DWORD i = 0; pTable[i] != 0x7fffffff; ++i )
    {
        if( pTable[i] == dwConst )
            return i;
    }

    // Unknown constant?
    assert(0);
    return 0;
}




//-----------------------------------------------------------------------------
// Name: IndexToConstant()
// Desc: Returns the constant stored at the given index
//-----------------------------------------------------------------------------
DWORD MeshElement::IndexToConstant( DWORD dwIndex, const DWORD* pTable ) // static
{
    return pTable[ dwIndex ];
}




//-----------------------------------------------------------------------------
// Name: ComputeFVFOffset()
// Desc: Compute an offset to the selected component of a vertex based on the FVF
//-----------------------------------------------------------------------------
DWORD MeshElement::ComputeFVFOffset( DWORD dwComponentFVF, DWORD dwFVF ) // static
{
    DWORD dwOffset = 0;

    if( dwComponentFVF >= D3DFVF_XYZ && dwComponentFVF <= D3DFVF_XYZB4 )
        return dwOffset;

    // Position (plus blend weights) offset.
    DWORD dwPositionFVF = dwFVF & D3DFVF_POSITION_MASK;

    dwOffset += 12;

    if( D3DFVF_XYZRHW == dwPositionFVF )
        dwOffset += 4;
    else if( dwPositionFVF >= D3DFVF_XYZB1 )
        dwOffset += 4 * ((dwPositionFVF-4)/2);

    if( D3DFVF_NORMAL == dwComponentFVF )
        return dwOffset;

    if( dwFVF & D3DFVF_NORMAL )
        dwOffset += 12;

    if( D3DFVF_DIFFUSE == dwComponentFVF )
        return dwOffset;

    if( dwFVF & D3DFVF_DIFFUSE )
        dwOffset += 4;

    if( D3DFVF_SPECULAR == dwComponentFVF )
        return dwOffset;

    if( dwFVF & D3DFVF_SPECULAR )
        dwOffset += 4;

    // Texture coordinates
    DWORD dwTexCount = (dwFVF & D3DFVF_TEXCOUNT_MASK) >> D3DFVF_TEXCOUNT_SHIFT;

    for( DWORD i = 0; i < dwTexCount; ++i )
    {
        if( D3DFVF_TEX1+i == dwComponentFVF )
            return dwOffset;

        DWORD dwFormat = (dwFVF >> (i*2 + 16)) & 0x03;

        if( D3DFVF_TEXTUREFORMAT1 == dwFormat )
            dwOffset += 4;
        else if( D3DFVF_TEXTUREFORMAT2 == dwFormat )
            dwOffset += 8;
        else if( D3DFVF_TEXTUREFORMAT3 == dwFormat )
            dwOffset += 12;
        else if( D3DFVF_TEXTUREFORMAT4 == dwFormat )
            dwOffset += 16;
    }

    // dwComponentFVF not found
    return dwOffset;
}




//-----------------------------------------------------------------------------
// Name: Mesh()
// Desc: Create empty mesh
//-----------------------------------------------------------------------------
Mesh::Mesh()
:
    m_iNumMeshElements( 0 ),
    m_Elements( NULL )
{
}




//-----------------------------------------------------------------------------
// Name: Mesh()
// Desc: Create mesh of given size
//-----------------------------------------------------------------------------
Mesh::Mesh( INT iNumElements )
:
    m_iNumMeshElements( iNumElements ),
    m_Elements( new MeshElement [iNumElements] )
{
}




//-----------------------------------------------------------------------------
// Name: ~Mesh()
// Desc: Destroy mesh
//-----------------------------------------------------------------------------
Mesh::~Mesh()
{
    delete [] m_Elements;
}




//-----------------------------------------------------------------------------
// Name: Save()
// Desc: Save all mesh elements to the file
//-----------------------------------------------------------------------------
HRESULT Mesh::Save( const File& file )
{
    if( !file.Write( &m_v3BoxMin, sizeof(m_v3BoxMin) ) )
        return E_FAIL;

    if( !file.Write( &m_v3BoxMax, sizeof(m_v3BoxMax) ) )
        return E_FAIL;

    if( !file.Write( &m_iNumMeshElements, sizeof(m_iNumMeshElements) ) )
        return E_FAIL;

    for( INT i = 0; i < m_iNumMeshElements; ++i )
    {
        if( FAILED( m_Elements[i].Save( file ) ) )
            return E_FAIL;
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: LoadSave()
// Desc: Load a mesh
//-----------------------------------------------------------------------------
HRESULT Mesh::Load( const File& file )
{
    DWORD dwBytesRead;

    if( !file.Read( &m_v3BoxMin, sizeof(m_v3BoxMin), dwBytesRead ) )
        return E_FAIL;

    if( !file.Read( &m_v3BoxMax, sizeof(m_v3BoxMax), dwBytesRead ) )
        return E_FAIL;

    if( !file.Read( &m_iNumMeshElements, sizeof(m_iNumMeshElements), dwBytesRead ) )
        return E_FAIL;

    delete [] m_Elements;

    m_Elements = new MeshElement [m_iNumMeshElements];

    for( INT i = 0; i < m_iNumMeshElements; ++i )
    {
        if( m_Elements[i].Load( file ) != S_OK )
            return E_FAIL;
    }

    return 0;
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Render the mesh
//-----------------------------------------------------------------------------
VOID Mesh::Render()
{
    // Render each mesh element
    for( INT i = 0; i < m_iNumMeshElements; ++i )
        m_Elements[i].Render();
}



//-----------------------------------------------------------------------------
// Name: AddBasisVectors()
// Desc: Add and calculate basis vector for the mesh
//-----------------------------------------------------------------------------
VOID Mesh::AddBasisVectors()
{
    for( INT i = 0; i < m_iNumMeshElements; ++i )
    {
        m_Elements[i].AddBasisVectors();
    }
}




//-----------------------------------------------------------------------------
// Name: Box()
// Desc: Create box mesh
//-----------------------------------------------------------------------------
Box::Box( FLOAT width, FLOAT height, FLOAT depth, const D3DMATERIAL8& d3dMat )
: 
    Mesh( 1 )
{
    MeshElement& elem = m_Elements[0];
    elem.m_pShader = new ShaderLit( d3dMat );
    elem.m_uNumPrimitives = 12;
    elem.m_uIndexBufferCount = 12*3;

    UINT uIndexBufferSize = sizeof(WORD) * elem.m_uIndexBufferCount;
    HRESULT hr = g_pd3dDevice->CreateIndexBuffer( uIndexBufferSize, D3DUSAGE_WRITEONLY, 
                                                  D3DFMT_INDEX16, D3DPOOL_MANAGED, 
                                                  &elem.m_pIndexBuffer );
    assert( hr == S_OK );

    struct SimpleVertex
    {
        D3DXVECTOR3 pos;
        D3DXVECTOR3 norm;
    };
    
    elem.m_uNumVertices = 4*6;
    elem.m_uVertexSize = sizeof(SimpleVertex);
    
    UINT uVertexBufferSize = elem.m_uVertexSize * elem.m_uNumVertices;
    hr = g_pd3dDevice->CreateVertexBuffer( uVertexBufferSize, D3DUSAGE_WRITEONLY, 
                                           (D3DFVF_XYZ | D3DFVF_NORMAL), 
                                           D3DPOOL_MANAGED, &elem.m_pVertexBuffer );
    assert( hr == S_OK );

    // Fill vertex buffer
    SimpleVertex* pVerts = NULL;
    elem.m_pVertexBuffer->Lock(0, uVertexBufferSize, (BYTE**)&pVerts, 0 );

    FLOAT hh = height * 0.5f;
    FLOAT hw = width * 0.5f;
    FLOAT hd = depth * 0.5f;

    // Bottom
    pVerts[0].pos  = D3DXVECTOR3( -hw, -hh, -hd );
    pVerts[0].norm = D3DXVECTOR3( 0.0f, -1.0f, 0.0f );
    pVerts[1].pos  = D3DXVECTOR3( hw, -hh, -hd );
    pVerts[1].norm = D3DXVECTOR3( 0.0f, -1.0f, 0.0f );
    pVerts[2].pos  = D3DXVECTOR3( hw, -hh, hd );
    pVerts[2].norm = D3DXVECTOR3( 0.0f, -1.0f, 0.0f );
    pVerts[3].pos  = D3DXVECTOR3( -hw, -hh, hd );
    pVerts[3].norm = D3DXVECTOR3( 0.0f, -1.0f, 0.0f );

    // Top
    pVerts[4].pos  = D3DXVECTOR3( -hw, hh, -hd );
    pVerts[4].norm = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
    pVerts[5].pos  = D3DXVECTOR3( -hw, hh, hd );
    pVerts[5].norm = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
    pVerts[6].pos  = D3DXVECTOR3( hw, hh, hd );
    pVerts[6].norm = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
    pVerts[7].pos  = D3DXVECTOR3( hw, hh, -hd );
    pVerts[7].norm = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );

    // Left
    pVerts[8].pos   = D3DXVECTOR3( -hw, -hh, -hd );
    pVerts[8].norm  = D3DXVECTOR3( -1.0f, 0.0f, 0.0f );
    pVerts[9].pos   = D3DXVECTOR3( -hw, -hh, hd );
    pVerts[9].norm  = D3DXVECTOR3( -1.0f, 0.0f, 0.0f );
    pVerts[10].pos  = D3DXVECTOR3( -hw, hh, hd );
    pVerts[10].norm = D3DXVECTOR3( -1.0f, 0.0f, 0.0f );
    pVerts[11].pos  = D3DXVECTOR3( -hw, hh, -hd );
    pVerts[11].norm = D3DXVECTOR3( -1.0f, 0.0f, 0.0f );

    // Right
    pVerts[12].pos  = D3DXVECTOR3( hw, -hh, -hd );
    pVerts[12].norm = D3DXVECTOR3( 1.0f, 0.0f, 0.0f );
    pVerts[13].pos  = D3DXVECTOR3( hw, hh, -hd );
    pVerts[13].norm = D3DXVECTOR3( 1.0f, 0.0f, 0.0f );
    pVerts[14].pos  = D3DXVECTOR3( hw, hh, hd );
    pVerts[14].norm = D3DXVECTOR3( 1.0f, 0.0f, 0.0f );
    pVerts[15].pos  = D3DXVECTOR3( hw, -hh, hd );
    pVerts[15].norm = D3DXVECTOR3( 1.0f, 0.0f, 0.0f );

    // Front
    pVerts[16].pos  = D3DXVECTOR3( -hw, -hh, -hd );
    pVerts[16].norm = D3DXVECTOR3( 0.0f, 0.0f, -1.0f );
    pVerts[17].pos  = D3DXVECTOR3( -hw, hh, -hd );
    pVerts[17].norm = D3DXVECTOR3( 0.0f, 0.0f, -1.0f );
    pVerts[18].pos  = D3DXVECTOR3( hw, hh, -hd );
    pVerts[18].norm = D3DXVECTOR3( 0.0f, 0.0f, -1.0f );
    pVerts[19].pos  = D3DXVECTOR3( hw, -hh, -hd );
    pVerts[19].norm = D3DXVECTOR3( 0.0f, 0.0f, -1.0f );

    // Back
    pVerts[20].pos  = D3DXVECTOR3( -hw, -hh, hd );
    pVerts[20].norm = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
    pVerts[21].pos  = D3DXVECTOR3( hw, -hh, hd );
    pVerts[21].norm = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
    pVerts[22].pos  = D3DXVECTOR3( hw, hh, hd );
    pVerts[22].norm = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
    pVerts[23].pos  = D3DXVECTOR3( -hw, hh, hd );
    pVerts[23].norm = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );

    elem.m_pVertexBuffer->Unlock();

    // Fill index buffer
    WORD* pIndices = 0;
    elem.m_pIndexBuffer->Lock(0, uIndexBufferSize, (BYTE**)&pIndices, 0 );

    // All verts are clockwise for each face
    for( WORD face = 0; face < 6; ++face )
    {
        WORD base = face * 4;
        *pIndices++ = base+0; 
        *pIndices++ = base+1; 
        *pIndices++ = base+3;
        *pIndices++ = base+1; 
        *pIndices++ = base+2; 
        *pIndices++ = base+3;
    }

    elem.m_pIndexBuffer->Unlock();
}




//-----------------------------------------------------------------------------
// Name: Sphere()
// Desc: Create sphere mesh
//-----------------------------------------------------------------------------
Sphere::Sphere( FLOAT radius, INT numMajor, INT numMinor, const D3DMATERIAL8& d3dMat )
: 
    Mesh( 1 )
{
    FLOAT majorStep = (D3DX_PI / numMajor);
    FLOAT minorStep = (2.0f * D3DX_PI / numMinor);

    MeshElement& elem = m_Elements[0];
    elem.m_pShader = new ShaderLit( d3dMat );

    INT NumStrips = numMajor;
    INT StripSize = (numMinor+1)*2;

    elem.m_PrimType = D3DPT_TRIANGLESTRIP;
    elem.m_uIndexBufferCount = NumStrips * StripSize + (NumStrips - 1) * 2;
    elem.m_uNumPrimitives = elem.m_uIndexBufferCount - 2;

    UINT uIndexBufferSize = sizeof(WORD) * elem.m_uIndexBufferCount;
    HRESULT hr = g_pd3dDevice->CreateIndexBuffer( uIndexBufferSize, D3DUSAGE_WRITEONLY, 
                                                  D3DFMT_INDEX16, D3DPOOL_MANAGED, 
                                                  &elem.m_pIndexBuffer );
    assert( hr == S_OK );

    struct SimpleVertex
    {
        D3DXVECTOR3 pos;
        D3DXVECTOR3 norm;
    };
    
    elem.m_uNumVertices = (numMajor+1) * (numMinor+1);
    elem.m_uVertexSize = sizeof(SimpleVertex);
    
    UINT uVertexBufferSize = elem.m_uVertexSize * elem.m_uNumVertices;
    hr = g_pd3dDevice->CreateVertexBuffer( uVertexBufferSize, D3DUSAGE_WRITEONLY, 
                                           (D3DFVF_XYZ | D3DFVF_NORMAL), 
                                            D3DPOOL_MANAGED, &elem.m_pVertexBuffer );
    assert( hr == S_OK );

    // Fill vertex buffer
    SimpleVertex* pVerts = NULL;
    elem.m_pVertexBuffer->Lock(0, uVertexBufferSize, (BYTE**)&pVerts, 0 );

    INT k = 0;

    // Build verts
    INT i;
    for( i = 0; i <= numMajor; ++i ) 
    {
        FLOAT a = i * majorStep;
        FLOAT r0 = radius * sinf(a);
        FLOAT z0 = radius * cosf(a);

        for( INT j = 0; j <= numMinor; ++j )
        {
            FLOAT c = j * minorStep;
            FLOAT x = cosf(c);
            FLOAT y = sinf(c);

            pVerts[k].pos.x = x * r0;
            pVerts[k].pos.y = y * r0;
            pVerts[k].pos.z = z0;

            //tx_coords[k][0] = FLOAT(j) / numMinor;
            //tx_coords[k][1] = FLOAT(i) / numMajor;

            FLOAT nx = (x * r0) / radius;
            FLOAT ny = (y * r0) / radius;
            FLOAT nz = z0 / radius;

            pVerts[k].norm.x = nx;
            pVerts[k].norm.y = ny;
            pVerts[k].norm.z = nz;

            //if( ny < 0.0f )
            //    ny = 0.0f;
            //INT color = 50 + INT(ny * 200);

            //colors[k][0] = (unsigned CHAR)color;
            //colors[k][1] = (unsigned CHAR)color;
            //colors[k][2] = (unsigned CHAR)color;
            ++k;
        }
    }

    elem.m_pVertexBuffer->Unlock();

    // Fill index buffer
    WORD* pIndices = NULL;
    elem.m_pIndexBuffer->Lock(0, uIndexBufferSize, (BYTE**)&pIndices, 0 );

    k = 0;

    // Build strips
    for( i = 0; i < numMajor; ++i )
    {
        if( i > 0 )
        {
            // Stitch strips together.
            pIndices[k] = pIndices[k-1];
            ++k;

            pIndices[k++] = WORD( (i)*(numMinor+1) );
        }

        for( INT j = 0; j <= numMinor; ++j )
        {
            // i,j
            pIndices[k++] = WORD( (i)*(numMinor+1) + j );

            // (i+1), j
            pIndices[k++] = WORD( (i+1)*(numMinor+1) + j );
        }
    }

    elem.m_pIndexBuffer->Unlock();
}
